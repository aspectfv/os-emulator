#include "Emulator.hpp"
#include "instructions/Print.hpp"
#include "schedulers/FCFSSCheduler.hpp"
#include "schedulers/RRScheduler.hpp"
#include <chrono>
#include <fstream>
#include <iostream>
#include <stdexcept>

Emulator::Emulator() {
  parser_.registerCommand(
      "initialize",
      [this](const std::vector<std::string> &args) { this->initialize(); });

  parser_.registerCommand(
      "exit", [this](const std::vector<std::string> &args) { this->exit(); });

  parser_.registerCommand(
      "screen",
      [this](const std::vector<std::string> &args) { this->screen(args); });

  parser_.registerCommand("scheduler-start",
                          [this](const std::vector<std::string> &args) {
                            this->scheduler_start();
                          });

  parser_.registerCommand(
      "scheduler-stop",
      [this](const std::vector<std::string> &args) { this->scheduler_stop(); });

  parser_.registerCommand(
      "report-util",
      [this](const std::vector<std::string> &args) { this->report_util(); });

  parser_.registerCommand(
      "process-smi",
      [this](const std::vector<std::string> &args) { this->process_smi(); });
}

bool Emulator::process_input(const std::string &input) {
  try {
    parser_.executeCommand(input);
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;

    if (e.what() == std::string("Exiting Emulator..."))
      return false;
  }

  return true;
}

void Emulator::cycle(std::stop_token st) {
  while (!st.stop_requested()) {
    if (!cores_[0].is_idle()) {
      cores_[0].tick();
    }

    if (scheduler_->has_processes() && cores_[0].is_idle()) {
      auto next_process = scheduler_->get_next_process();
      cores_[0].set_current_process(next_process);
    }

    if (scheduler_->is_running())
      generate_process();

    // sleep to prevent process from terminating too fast
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    cpu_cycles_++;

    // cycle finished -> notify waiting thread
    {
      std::lock_guard<std::mutex> lock(mtx_);
      cycle_finished_ = true;
      cv_.notify_one();
    }
  }
}

void Emulator::generate_process() {
  int batch_freq = config_.get_batch_process_freq();

  for (int i = 0; i < batch_freq; ++i) {
    std::string process_name = "p" + std::to_string(process_count_++);

    int num_instructions =
        config_.get_min_ins() +
        (rand() % (config_.get_max_ins() - config_.get_min_ins() + 1));

    std::unique_ptr<Process> process =
        std::make_unique<Process>(process_name, "00:00:00", num_instructions,
                                  config_.get_quantum_cycles());

    std::vector<std::unique_ptr<IInstruction>> instructions;

    for (int j = 0; j < num_instructions; ++j) {
      instructions.push_back(std::make_unique<Print>(
          process_name + ": Instruction " + std::to_string(j + 1)));
    }

    process->set_instructions(std::move(instructions));

    processes_[process_name] = std::move(process);
    scheduler_->add_process(processes_[process_name].get());
  }
}

void Emulator::initialize() {
  if (is_initialized_)
    throw std::runtime_error("Emulator is already initialized.");

  std::string config_file = "config.txt";

  if (!config_.load(config_file))
    throw std::runtime_error("Configuration not loaded.");

  for (int i = 0; i < config_.get_num_cpu(); i++) {
    cores_.emplace_back(i, config_.get_scheduler(),
                        config_.get_delay_per_exec());
  }

  std::string scheduler_type = config_.get_scheduler();

  if (scheduler_type == "fcfs") {
    scheduler_ = std::make_unique<FCFSScheduler>();
  } else if (scheduler_type == "rr") {
    int quantum_cycles = config_.get_quantum_cycles();
    scheduler_ = std::make_unique<RRScheduler>(quantum_cycles);
  } else {
    throw std::runtime_error("Unknown scheduler type: " + scheduler_type);
  }

  is_initialized_ = true;
  cycle_thread_ = std::jthread([this](std::stop_token st) { cycle(st); });
}

void Emulator::exit() {
  if (current_process_) {
    std::cout << "Exiting screen for process: " << current_process_->get_name()
              << std::endl;
    current_process_ = nullptr;
    return;
  }

  throw std::runtime_error("Exiting Emulator...");
}

void Emulator::screen(const std::vector<std::string> &args) {
  if (!is_initialized_)
    throw std::runtime_error("Emulator is not initialized.");

  if (args.empty())
    throw std::runtime_error("No screen command provided.");

  std::string arg = args[0];

  if (arg == "-ls") {
    std::cout << "Processes:\n";
    for (const auto &pair : processes_) {
      std::cout << "- " << pair.first << "\n";
    }
    std::cout << std::flush;
    return;
  }

  if (arg == "-s") {
    if (args.size() < 2)
      throw std::runtime_error("No process name provided for -s command.");

    std::string process_name = args[1];

    auto [it, inserted] = processes_.try_emplace(process_name, nullptr);

    if (inserted) {
      int num_instructions =
          config_.get_min_ins() +
          (rand() % (config_.get_max_ins() - config_.get_min_ins() + 1));

      std::unique_ptr<Process> new_process =
          std::make_unique<Process>(process_name, "00:00:00", num_instructions,
                                    config_.get_quantum_cycles());

      std::vector<std::unique_ptr<IInstruction>> instructions;

      for (int j = 0; j < num_instructions; ++j) {
        instructions.push_back(std::make_unique<Print>(
            process_name + ": Instruction " + std::to_string(j + 1)));
      }

      new_process->set_instructions(std::move(instructions));

      it->second = std::move(new_process);
    }

    Process *process = it->second.get();

    process->set_state(Process::ProcessState::READY);

    current_process_ = processes_[process_name].get();

    scheduler_->add_process(current_process_);

    std::cout << "\033[2J\033[1;1H";
  }
}

void Emulator::scheduler_start() {
  if (!is_initialized_)
    throw std::runtime_error("Emulator is not initialized.");

  if (scheduler_ && scheduler_->is_running())
    throw std::runtime_error("Scheduler is already running.");

  scheduler_->start();
}

void Emulator::scheduler_stop() {
  if (!is_initialized_)
    throw std::runtime_error("Emulator is not initialized.");

  scheduler_->stop();
}

void Emulator::report_util() {
  if (!is_initialized_)
    throw std::runtime_error("Emulator is not initialized.");

  {
    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait(lock, [this] { return cycle_finished_; });
    cycle_finished_ = false;
  }

  std::ofstream report_file("csopesy-log.txt");

  if (!report_file.is_open())
    throw std::runtime_error("Failed to open report file.");

  int busy_cores = 0;

  for (const auto &core : cores_) {
    if (!core.is_idle())
      busy_cores++;
  }

  int cpu_utilization = cores_.empty() ? 0 : (busy_cores * 100) / cores_.size();

  report_file.close();
}

void Emulator::process_smi() {
  if (!is_initialized_)
    throw std::runtime_error("Emulator is not initialized.");

  if (!current_process_)
    throw std::runtime_error("No current process to handle SMI.");

  std::cout << "Process Name: " << current_process_->get_name() << std::endl;
  std::cout << "ID: " << current_process_->get_id() << std::endl;
  std::cout << "Logs: " << std::endl;

  for (const auto &log : current_process_->get_logs()) {
    std::cout << "(" << log.timestamp << ") "
              << "Core " << log.core_id << ": " << "\"" << log.message << "\""
              << std::endl;
  }

  if (current_process_->is_finished()) {
    std::cout << "Finished!" << std::endl;
  } else {
    std::cout << "Current instruction line: "
              << current_process_->get_instruction_pointer() << std::endl;

    std::cout << "Lines of code: " << current_process_->get_total_instructions()
              << std::endl;
  }
}

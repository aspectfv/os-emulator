#include "Emulator.hpp"
#include "Process.hpp"
#include "instructions/InstructionFactory.hpp"
#include "schedulers/SchedulerFactory.hpp"
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
    // assign processes to idle cores
    assign_processes();

    // tick all cores and collect returned processes
    std::vector<Process *> returned_processes = tick_cores();

    // generate new processes if scheduler is running
    if (scheduler_->is_running())
      generate_processes();

    // handle returned processes
    handle_returned_processes(returned_processes);

    // handle sleeping processes
    handle_sleeping_processes();

    // sleep to prevent process from terminating too fast for debugging
    // std::this_thread::sleep_for(std::chrono::milliseconds(300)); // commented out in actual demo for accuracy purposes
    cpu_cycles_++;

    // cycle finished -> notify waiting thread
    {
      std::lock_guard<std::mutex> lock(mtx_);
      cycle_finished_ = true;
      cv_.notify_one();
    }
  }
}

void Emulator::assign_processes() {
  for (auto &core : cores_) {
    if (core.is_idle() && scheduler_->has_processes()) {
      Process *next_process = scheduler_->get_next_process();
      core.set_current_process(next_process);
    }
  }
}

std::vector<Process *> Emulator::tick_cores() {
  std::vector<Process *> returned_processes(cores_.size());

  // tick all cores in parallel
  // auto join after scope ends
  {
    std::vector<std::jthread> core_threads;
    core_threads.reserve(cores_.size());

    for (size_t i = 0; i < cores_.size(); ++i) {
      if (!cores_[i].is_idle()) {
        core_threads.emplace_back([this, i, &returned_processes]() {
          returned_processes[i] = cores_[i].tick();
        });
      }
    }
  }

  return returned_processes;
}

void Emulator::generate_processes() {
  int batch_freq = config_.get_batch_process_freq();

  // generate batch_freq amount of processes
  for (int i = 0; i < batch_freq; ++i) {
    std::string process_name = "p" + std::to_string(process_count_++);

    int num_instructions =
        config_.get_min_ins() +
        (rand() % (config_.get_max_ins() - config_.get_min_ins() + 1));

    std::vector<std::unique_ptr<IInstruction>> instructions =
        InstructionFactory::create_instructions(process_name, num_instructions,
                                                config_.get_max_ins(),
                                                config_.get_min_ins());

    std::unique_ptr<Process> process = std::make_unique<Process>(
        process_name, num_instructions, config_.get_quantum_cycles());

    process->set_instructions(std::move(instructions));

    processes_[process_name] = std::move(process);
    scheduler_->add_process(processes_[process_name].get());
  }
}

void Emulator::handle_returned_processes(
    const std::vector<Process *> &returned_processes) {

  for (size_t i = 0; i < returned_processes.size(); ++i) {
    Process *returned_process = returned_processes[i];

    if (!returned_process)
      continue;

    switch (returned_process->get_state()) {
      case Process::ProcessState::TERMINATED:
        terminated_processes_.push_back(returned_process);
        break;
      case Process::ProcessState::READY: {
        scheduler_->add_process(returned_process);

        if (scheduler_->has_processes()) {
          Process *next_process = scheduler_->get_next_process();
          cores_[i].set_current_process(next_process);
        }

        break;
      }
      case Process::ProcessState::SLEEPING:
        sleeping_processes_.push_back(returned_process);
        break;
      default:
        break;
    }
  }
}

void Emulator::handle_sleeping_processes() {
  auto it = std::remove_if(sleeping_processes_.begin(),
                           sleeping_processes_.end(), [this](Process *process) {
                             process->decrement_sleep_ticks();
                             if (process->get_state() ==
                                 Process::ProcessState::READY) {
                               scheduler_->add_process(process, true);
                               return true;
                             }
                             return false;
                           });
  sleeping_processes_.erase(it, sleeping_processes_.end());
}

void Emulator::initialize() {
  if (is_initialized_)
    throw std::runtime_error("Emulator is already initialized.");

  std::string config_file = "config.txt";

  if (!config_.load(config_file))
    throw std::runtime_error("Configuration not loaded.");

  for (int i = 0; i < config_.get_num_cpu(); ++i) {
    cores_.emplace_back(i, config_.get_scheduler(),
                        config_.get_delay_per_exec());
  }

  std::string scheduler = config_.get_scheduler();
  scheduler_ = SchedulerFactory::create_scheduler(scheduler,
                                                  config_.get_quantum_cycles());

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

  if (arg == "-s") {
    start_screen(const_cast<std::vector<std::string> &>(args));
    return;
  }

  if (arg == "-ls") {
    list_screens();
    return;
  }

  if (arg == "-r") {
    resume_screen(const_cast<std::vector<std::string> &>(args));
    return;
  }
}

void Emulator::start_screen(std::vector<std::string> &args) {
  if (args.size() < 2)
    throw std::runtime_error("No process name provided for -s command.");

  std::string process_name = args[1];

  auto [it, inserted] = processes_.try_emplace(process_name, nullptr);

  if (inserted) {
    int num_instructions =
        config_.get_min_ins() +
        (rand() % (config_.get_max_ins() - config_.get_min_ins() + 1));

    std::vector<std::unique_ptr<IInstruction>> instructions =
        InstructionFactory::create_instructions(process_name, num_instructions,
                                                config_.get_max_ins(),
                                                config_.get_min_ins());

    std::unique_ptr<Process> new_process = std::make_unique<Process>(
        process_name, num_instructions, config_.get_quantum_cycles());

    new_process->set_instructions(std::move(instructions));

    it->second = std::move(new_process);
  }

  Process *process = it->second.get();

  process->set_state(Process::ProcessState::READY);

  current_process_ = processes_[process_name].get();

  scheduler_->add_process(current_process_);

  std::cout << "\033[2J\033[1;1H";
}

void Emulator::list_screens() {
  // wait for cpu cycle to finish for accurate report
  {
    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait(lock, [this] { return cycle_finished_; });
    cycle_finished_ = false;
  }
  // does the same as report_util but doesnt log to a file
  log_cpu_util_report(std::cout);
}

void Emulator::resume_screen(std::vector<std::string> &args) {
  if (args.size() < 2)
    throw std::runtime_error("No process name provided for -r command.");

  std::string process_name = args[1];

  auto it = processes_.find(process_name);

  if (it == processes_.end()) {
    std::string error_msg = "Process " + process_name + " not found.";
    throw std::runtime_error(error_msg);
    return;
  }

  current_process_ = it->second.get();

  std::cout << "\033[2J\033[1;1H";
}

void Emulator::scheduler_start() {
  if (!is_initialized_)
    throw std::runtime_error("Emulator is not initialized.");

  if (scheduler_ && scheduler_->is_running())
    throw std::runtime_error("Scheduler is already running.");

  scheduler_->start();

  std::cout << "Scheduler started." << std::endl;
}

void Emulator::scheduler_stop() {
  if (!is_initialized_)
    throw std::runtime_error("Emulator is not initialized.");

  scheduler_->stop();

  std::cout << "Scheduler stopped." << std::endl;
}

void Emulator::report_util() {
  if (!is_initialized_)
    throw std::runtime_error("Emulator is not initialized.");

  // wait for cpu cycle to finish for accurate report
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

  log_cpu_util_report(std::cout);
  log_cpu_util_report(report_file);

  report_file.close();
}

void Emulator::log_cpu_util_report(std::ostream &output_stream) {
  int busy_cores = 0;

  for (const auto &core : cores_) {
    if (!core.is_idle())
      busy_cores++;
  }

  int cpu_utilization = cores_.empty() ? 0 : (busy_cores * 100) / cores_.size();

  output_stream << "CPU utilization: " << cpu_utilization << "%\n";
  output_stream << "Cores used: " << busy_cores << "\n";
  output_stream << "Cores available: " << cores_.size() << "\n\n";
  output_stream << "----------------------------------------\n";
  output_stream << "Running processes:\n";

  bool any_running = false;

  auto format_process_log = [](const Process *process, int core_id) {
    std::string core_info =
        core_id < 0 ? "Finished" : "Core: " + std::to_string(core_id);
    return process->get_name() + "  " + "(" + process->get_created_at() +
           ")  " + core_info + "  " +
           std::to_string(process->get_instruction_pointer()) + "/" +
           std::to_string(process->get_total_instructions()) + "\n";
  };

  for (size_t i = 0; i < cores_.size(); ++i) {
    if (!cores_[i].is_idle()) {
      const Process *running_process = cores_[i].get_current_process();
      if (!running_process)
        continue;

      any_running = true;
      output_stream << format_process_log(running_process, i);
    }
  }

  if (!any_running)
    output_stream << "No running processes.\n";

  output_stream << std::endl;

  output_stream << "Finished processes:\n";

  if (terminated_processes_.empty()) {
    output_stream << "No finished processes.\n";
  } else {
    for (const auto &finished_process : terminated_processes_) {
      output_stream << format_process_log(finished_process, -1);
    }
  }

  output_stream << "----------------------------------------\n\n";
}

void Emulator::process_smi() {
  if (!is_initialized_)
    throw std::runtime_error("Emulator is not initialized.");

  if (!current_process_)
    throw std::runtime_error("No current process to handle SMI.");

  std::cout << "Process Name: " << current_process_->get_name() << std::endl;
  std::cout << "ID: " << current_process_->get_id() << std::endl;
  std::cout << "Logs: " << std::endl;

  for (const ProcessLog &log : current_process_->get_logs()) {
    std::cout << "(" << log.timestamp << ") "
              << "Core " << log.core_id << ": " << "\"" << log.message << "\""
              << std::endl;
  }

  if (current_process_->get_state() == Process::ProcessState::TERMINATED) {
    std::cout << "Finished!" << std::endl;
  } else {
    std::cout << "Current instruction line: "
              << current_process_->get_instruction_pointer() << std::endl;

    std::cout << "Lines of code: " << current_process_->get_total_instructions()
              << std::endl;
  }
}

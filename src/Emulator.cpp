#include "Emulator.hpp"
#include "instructions/Print.hpp"
#include "schedulers/FCFSSCheduler.hpp"
#include "schedulers/RRScheduler.hpp"
#include <cstdlib>
#include <iostream>
#include <stdexcept>

Emulator::Emulator() {
  parser_.registerCommand("exit", [this](const std::string &args) { exit(); });
  parser_.registerCommand("initialize",
                          [this](const std::string &args) { initialize(); });
  parser_.registerCommand("screen",
                          [this](const std::string &args) { screen(args); });
  parser_.registerCommand("scheduler-start", [this](const std::string &args) {
    scheduler_start();
  });
  parser_.registerCommand(
      "scheduler-stop", [this](const std::string &args) { scheduler_stop(); });
  parser_.registerCommand("report-util",
                          [this](const std::string &args) { report_util(); });
  parser_.registerCommand("process-smi",
                          [this](const std::string &args) { process_smi(); });
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

    if (scheduler_->is_running()) {
      int batch_freq = config_.get_batch_process_freq();

      for (int i = 0; i < batch_freq; ++i) {
        std::string process_name = "p" + std::to_string(cpu_cycles_);

        auto process =
            std::make_unique<Process>(cpu_cycles_, process_name, "00:00:00");

        std::vector<std::unique_ptr<IInstruction>> instructions;

        int num_instructions = rand() % 10 + 1; // Random between 1 and 10

        for (int j = 0; j < num_instructions; ++j) {
          instructions.push_back(std::make_unique<Print>(
              process_name + ": Instruction " + std::to_string(j + 1)));
        }

        process->set_instructions(std::move(instructions));

        processes_[process_name] = std::move(process);
        scheduler_->add_process(processes_[process_name].get());
      }
    }

    // sleep to prevent process from terminating too fast
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    cpu_cycles_++;
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

void Emulator::exit() { throw std::runtime_error("Exiting Emulator..."); }

void Emulator::screen(const std::string &args) {}

void Emulator::scheduler_start() {
  if (!is_initialized_)
    throw std::runtime_error("Emulator is not initialized.");

  if (scheduler_ && scheduler_->is_running())
    throw std::runtime_error("Scheduler is already running.");

  scheduler_->start();
}

void Emulator::scheduler_stop() {}

void Emulator::report_util() {}

void Emulator::process_smi() {}

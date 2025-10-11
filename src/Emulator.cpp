#include "Emulator.hpp"
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

void Emulator::cycle() {
  while (is_initialized_) {
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

  is_initialized_ = true;
  cycle_thread_ = std::jthread(&Emulator::cycle, this);
}

void Emulator::exit() {
  is_initialized_ = false;

  throw std::runtime_error("Exiting Emulator...");
}

void Emulator::screen(const std::string &args) {}

void Emulator::scheduler_start() {}

void Emulator::scheduler_stop() {}

void Emulator::report_util() {}

void Emulator::process_smi() {}

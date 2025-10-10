#include "Emulator.hpp"
#include <iostream>

Emulator::Emulator() {
  parser_.registerCommand("exit", [this](const std::string &args) { exit(); });
  is_initialized_ = true;
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

void Emulator::initialize() {
  if (is_initialized_)
    throw std::runtime_error("Emulator is already initialized.");

  std::string config_file = "config.txt";

  if (!config_.load(config_file))
    throw std::runtime_error("Configuration not loaded.");

  is_initialized_ = true;
}
void Emulator::exit() { throw std::runtime_error("Exiting Emulator..."); }

#include "Emulator.hpp"
#include <iostream>

Emulator::Emulator() {
  parser.registerCommand("exit", [this](const std::string &args) { exit(); });
  is_initialized = true;
}

bool Emulator::process_input(const std::string &input) {
  try {
    parser.executeCommand(input);
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;

    if (e.what() == std::string("Exiting Emulator..."))
      return false;
  }

  return true;
}

void Emulator::exit() { throw std::runtime_error("Exiting Emulator..."); }

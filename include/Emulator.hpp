#pragma once
#include "CommandParser.hpp"
#include <string>

class Emulator {
public:
  Emulator();
  bool process_input(const std::string &input);

private:
  CommandParser parser;
  bool is_initialized = false;

  void exit();
};

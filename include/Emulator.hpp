#pragma once
#include "CommandParser.hpp"
#include "Config.hpp"
#include <string>

class Emulator {
public:
  Emulator();
  bool process_input(const std::string &input);

private:
  CommandParser parser;
  Config &config = Config::instance();
  bool is_initialized = false;

  void initialize();
  void exit();
};

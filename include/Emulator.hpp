#pragma once
#include "CommandParser.hpp"
#include "Config.hpp"
#include <string>
#include <thread>

class Emulator {
public:
  Emulator();
  bool process_input(const std::string &input);

private:
  CommandParser parser_;
  Config &config_ = Config::instance();
  bool is_initialized_ = false;
  std::thread cycle_thread_;
  int cpu_cycles_ = 0;

  void cycle();
  void initialize();
  void exit();
};

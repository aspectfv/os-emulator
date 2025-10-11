#pragma once
#include "CommandParser.hpp"
#include "Config.hpp"
#include "Process.hpp"
#include <memory>
#include <string>
#include <thread>
#include <vector>

class Emulator {
public:
  Emulator();
  bool process_input(const std::string &input);

private:
  CommandParser parser_;
  Config &config_ = Config::instance();
  bool is_initialized_ = false;
  std::vector<std::unique_ptr<Process>> processes_;
  std::thread cycle_thread_;
  int cpu_cycles_ = 0;

  void cycle();
  void initialize();
  void exit();
};

#pragma once
#include "CPUCore.hpp"
#include "CommandParser.hpp"
#include "Config.hpp"
#include "Process.hpp"
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

class Emulator {
public:
  Emulator();
  bool process_input(const std::string &input);

private:
  CommandParser parser_;
  Config &config_ = Config::instance();
  bool is_initialized_ = false;
  std::unordered_map<std::string, std::unique_ptr<Process>> processes_;
  std::thread cycle_thread_;
  int cpu_cycles_ = 0;

  // single core for now
  std::vector<CPUCore> cores_;

  // command handlers
  void cycle();
  void initialize();
  void exit();
  void screen(const std::string &args);
  void scheduler_start();
  void scheduler_stop();
  void report_util();

  // screen command handlers
  void process_smi();
};

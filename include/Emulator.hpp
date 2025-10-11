#pragma once
#include "CPUCore.hpp"
#include "CommandParser.hpp"
#include "Config.hpp"
#include "Process.hpp"
#include "schedulers/IScheduler.hpp"
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

  // process management
  std::unordered_map<std::string, std::unique_ptr<Process>> processes_;
  std::vector<Process *> terminated_processes_;

  std::jthread cycle_thread_;
  int cpu_cycles_ = 0;

  std::vector<CPUCore> cores_;

  std::unique_ptr<IScheduler> scheduler_;
  int process_count_ = 0;

  // cycle mutex
  std::mutex mtx_;
  std::condition_variable cv_;
  bool cycle_finished_ = false;

  // cpu cycle loop
  void cycle(std::stop_token st);
  void generate_process();

  // command handlers
  void initialize();
  void exit();
  void screen(const std::string &args);
  void scheduler_start();
  void scheduler_stop();
  void report_util();

  // screen command handlers
  void process_smi();
};

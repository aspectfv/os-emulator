#pragma once
#include "CPUCore.hpp"
#include "CommandParser.hpp"
#include "Config.hpp"
#include "MemoryManager.hpp"
#include "Process.hpp"
#include "schedulers/IScheduler.hpp"
#include <condition_variable>
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
  std::vector<Process *> sleeping_processes_;
  Process *current_process_ = nullptr;

  int cpu_cycles_ = 0;

  std::vector<CPUCore> cores_;

  std::unique_ptr<IScheduler> scheduler_;
  int process_count_ = 0;

  // cycle mutex
  std::mutex mtx_;
  std::condition_variable cv_;
  bool cycle_finished_ = false;
  std::jthread cycle_thread_;

  std::unique_ptr<MemoryManager> memory_manager_;

  // cpu stats
  uint64_t total_cpu_ticks_ = 0;
  uint64_t idle_cpu_ticks_ = 0;
  uint64_t active_cpu_ticks_ = 0;

  // cpu cycle loop
  void cycle(std::stop_token st);

  // cycle helpers
  uint32_t assign_processes(uint32_t current_running_processes,
                            uint32_t max_concurrent_processes);
  std::vector<Process *> tick_cores();
  void generate_processes();
  void
  handle_returned_processes(const std::vector<Process *> &returned_processes);
  void handle_sleeping_processes();

  // command handlers
  void initialize();
  void exit();

  void screen(const std::vector<std::string> &args);

  // screen helpers
  void start_screen(std::vector<std::string> &args);
  void list_screens();
  void resume_screen(std::vector<std::string> &args);
  void custom_screen(std::vector<std::string> &args);

  void scheduler_start();
  void scheduler_stop();
  void report_util();

  // report helper
  void log_cpu_util_report(std::ostream &output_stream);

  // screen command handlers
  void process_smi();
  void vmstat();
};

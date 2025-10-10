#pragma once

#include <string>

class Config {
public:
  static Config &instance() {
    static Config instance;
    return instance;
  }
  void load(const std::string &filename);
  int get_num_cpu() const { return num_cpu; }
  std::string get_scheduler() const { return scheduler; }
  int get_quantum_cycles() const { return quantum_cycles; }
  int get_batch_process_freq() const { return batch_process_freq; }
  int get_min_ins() const { return min_ins; }
  int get_max_ins() const { return max_ins; }
  int get_delay_per_exec() const { return delay_per_exec; }

private:
  Config();
  ~Config() = default;

  // delete copy and move constructors and assign operators
  Config(const Config &) = delete;
  Config &operator=(const Config &) = delete;
  Config(Config &&) = delete;
  Config &operator=(Config &&) = delete;

  int num_cpu;
  std::string scheduler;
  int quantum_cycles;
  int batch_process_freq;
  int min_ins;
  int max_ins;
  int delay_per_exec;
};

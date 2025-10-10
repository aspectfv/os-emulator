#pragma once

#include <string>

class Config {
public:
  Config();
  void load(const std::string &filename);
  int get_num_cpu() { return num_cpu; }
  std::string get_scheduler() { return scheduler; }
  int get_quantum_cycles() { return quantum_cycles; }
  int get_batch_process_freq() { return batch_process_freq; }
  int get_min_ins() { return min_ins; }
  int get_max_ins() { return max_ins; }
  int get_delay_per_exec() { return delay_per_exec; }

private:
  int num_cpu;
  std::string scheduler;
  int quantum_cycles;
  int batch_process_freq;
  int min_ins;
  int max_ins;
  int delay_per_exec;
};

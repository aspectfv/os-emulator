#include "Config.hpp"
#include <fstream>
#include <iostream>

Config::Config() {
  num_cpu_ = 4;
  scheduler_ = "rr";
  quantum_cycles_ = 5;
  batch_process_freq_ = 1;
  min_ins_ = 1000;
  max_ins_ = 2000;
  delay_per_exec_ = 0;
}

bool Config::load(const std::string &filename) {
  std::ifstream config_file(filename);
  if (!config_file.is_open()) {
    std::cerr << "Failed to open config file: " << filename << std::endl;
    return false;
  }

  std::string key, val;
  while (config_file >> key >> val) {
    if (key == "num-cpu") {
      num_cpu_ = std::stoi(val);
    } else if (key == "scheduler") {
      scheduler_ = val;
    } else if (key == "quantum-cycles") {
      quantum_cycles_ = std::stoi(val);
    } else if (key == "batch-process-freq") {
      batch_process_freq_ = std::stoi(val);
    } else if (key == "min-ins") {
      min_ins_ = std::stoi(val);
    } else if (key == "max-ins") {
      max_ins_ = std::stoi(val);
    } else if (key == "delay-per-exec") {
      delay_per_exec_ = std::stoi(val);
    } else {
      std::cerr << "Unknown config key: " << key << std::endl;
    }
  }

  config_file.close();

  return true;
}

// getters
int Config::get_num_cpu() const { return num_cpu_; }
std::string Config::get_scheduler() const { return scheduler_; }
int Config::get_quantum_cycles() const { return quantum_cycles_; }
int Config::get_batch_process_freq() const { return batch_process_freq_; }
int Config::get_min_ins() const { return min_ins_; }
int Config::get_max_ins() const { return max_ins_; }
int Config::get_delay_per_exec() const { return delay_per_exec_; }

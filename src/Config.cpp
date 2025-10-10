#include "Config.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>

Config::Config() {
  num_cpu = 4;
  scheduler = "rr";
  quantum_cycles = 5;
  batch_process_freq = 1;
  min_ins = 1000;
  max_ins = 2000;
  delay_per_exec = 0;
}

void Config::load(const std::string &filename) {
  std::ifstream config_file(filename);
  if (!config_file.is_open()) {
    throw std::runtime_error("Could not open config file: " + filename);
  }

  std::string key, val;
  while (config_file >> key >> val) {
    if (key == "num-cpu") {
      num_cpu = std::stoi(val);
    } else if (key == "scheduler") {
      scheduler = val;
    } else if (key == "quantum-cycles") {
      quantum_cycles = std::stoi(val);
    } else if (key == "batch-process-freq") {
      batch_process_freq = std::stoi(val);
    } else if (key == "min-ins") {
      min_ins = std::stoi(val);
    } else if (key == "max-ins") {
      max_ins = std::stoi(val);
    } else if (key == "delay-per-exec") {
      delay_per_exec = std::stoi(val);
    } else {
      std::cerr << "Unknown config key: " << key << std::endl;
    }
  }

  config_file.close();
}

#include "Config.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>

Config::Config() {
  num_cpu_ = 4;
  scheduler_ = "rr";
  quantum_cycles_ = 5;
  batch_process_freq_ = 1;
  min_ins_ = 1000;
  max_ins_ = 2000;
  delay_per_exec_ = 0;

  max_overall_mem_ = 16384; // 2^14
  mem_per_frame_ = 256;     // 2^8
  min_mem_per_proc_ = 512;  // 2^9
  max_mem_per_proc_ = 2048; // 2^11
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
      int val_int = std::stoi(val);

      if (val_int < 1 || val_int > std::numeric_limits<uint8_t>::max()) {
        std::cerr << "Invalid number of CPUs: " << val
                  << ". Must be between 1 and "
                  << static_cast<int>(std::numeric_limits<uint8_t>::max())
                  << ". Clamping to valid range." << std::endl;
        num_cpu_ = static_cast<uint8_t>(
            std::clamp<int>(val_int, 1, std::numeric_limits<uint8_t>::max()));
        continue;
      }

      num_cpu_ = static_cast<uint8_t>(val_int);
    } else if (key == "scheduler") {
      std::string scheduler = val;

      scheduler.erase(std::remove(scheduler.begin(), scheduler.end(), '\"'),
                      scheduler.end()); // remove quotes if any
      //
      if (scheduler != "rr" && scheduler != "fcfs") {
        std::cerr << "Invalid scheduler type: " << scheduler
                  << ". Defaulting to 'rr'." << std::endl;
        scheduler_ = "rr";
      }

      scheduler_ = scheduler;
    } else if (key == "quantum-cycles") {
      uint64_t val_ul = std::stoul(val);

      if (val_ul < 1 || val_ul > std::numeric_limits<uint32_t>::max()) {
        std::cerr << "Invalid quantum cycles: " << val
                  << ". Must be between 1 and "
                  << std::numeric_limits<uint32_t>::max()
                  << ". Clamping to valid range." << std::endl;
        quantum_cycles_ = static_cast<uint32_t>(std::clamp<uint64_t>(
            val_ul, 1, std::numeric_limits<uint32_t>::max()));
        continue;
      }

      quantum_cycles_ = static_cast<uint32_t>(val_ul);
    } else if (key == "batch-process-freq") {
      uint64_t val_ul = std::stoul(val);

      if (val_ul < 1 || val_ul > std::numeric_limits<uint32_t>::max()) {
        std::cerr << "Invalid batch process frequency: " << val
                  << ". Must be between 1 and "
                  << std::numeric_limits<uint32_t>::max()
                  << ". Clamping to valid range." << std::endl;
        batch_process_freq_ = static_cast<uint32_t>(std::clamp<uint64_t>(
            val_ul, 1, std::numeric_limits<uint32_t>::max()));
        continue;
      }

      batch_process_freq_ = static_cast<uint32_t>(val_ul);
    } else if (key == "min-ins") {
      uint64_t val_ul = std::stoul(val);

      if (val_ul < 1 || val_ul > std::numeric_limits<uint32_t>::max()) {
        std::cerr << "Invalid minimum instructions: " << val
                  << ". Must be between 1 and "
                  << std::numeric_limits<uint32_t>::max()
                  << ". Clamping to valid range." << std::endl;
        min_ins_ = static_cast<uint32_t>(std::clamp<uint64_t>(
            val_ul, 1, std::numeric_limits<uint32_t>::max()));
        continue;
      }

      min_ins_ = static_cast<uint32_t>(val_ul);
    } else if (key == "max-ins") {
      uint64_t val_ul = std::stoul(val);

      if (val_ul < 1 || val_ul > std::numeric_limits<uint32_t>::max()) {
        std::cerr << "Invalid maximum instructions: " << val
                  << ". Must be between 1 and "
                  << std::numeric_limits<uint32_t>::max()
                  << ". Clamping to valid range." << std::endl;
        max_ins_ = static_cast<uint32_t>(std::clamp<uint64_t>(
            val_ul, 1, std::numeric_limits<uint32_t>::max()));
        continue;
      }

      max_ins_ = static_cast<uint32_t>(val_ul);

      if (max_ins_ < min_ins_) {
        std::cerr << "Maximum instructions (" << max_ins_
                  << ") cannot be less than minimum instructions (" << min_ins_
                  << "). Adjusting maximum instructions to " << min_ins_ << "."
                  << std::endl;
        max_ins_ = min_ins_;
      }
    } else if (key == "delay-per-exec") {
      uint64_t val_ul = std::stoul(val);

      if (val_ul > std::numeric_limits<uint32_t>::max()) {
        std::cerr << "Invalid delay per execution: " << val
                  << ". Must be between 0 and "
                  << std::numeric_limits<uint32_t>::max()
                  << ". Clamping to valid range." << std::endl;
        delay_per_exec_ = static_cast<uint32_t>(std::clamp<uint64_t>(
            val_ul, 0, std::numeric_limits<uint32_t>::max()));
        continue;
      }

      delay_per_exec_ = static_cast<uint32_t>(val_ul);
    } else if (key == "max-overall-mem") {
      max_overall_mem_ = static_cast<uint32_t>(std::stoul(val));
    } else if (key == "mem-per-frame") {
      mem_per_frame_ = static_cast<uint32_t>(std::stoul(val));
    } else if (key == "min-mem-per-proc") {
      min_mem_per_proc_ = static_cast<uint32_t>(std::stoul(val));
    } else if (key == "max-mem-per-proc") {
      max_mem_per_proc_ = static_cast<uint32_t>(std::stoul(val));
    } else {
      std::cerr << "Unknown config key: " << key << std::endl;
    }
  }

  config_file.close();

  return true;
}

// getters
uint8_t Config::get_num_cpu() const { return num_cpu_; }
std::string Config::get_scheduler() const { return scheduler_; }
uint32_t Config::get_quantum_cycles() const { return quantum_cycles_; }
uint32_t Config::get_batch_process_freq() const { return batch_process_freq_; }
uint32_t Config::get_min_ins() const { return min_ins_; }
uint32_t Config::get_max_ins() const { return max_ins_; }
uint32_t Config::get_delay_per_exec() const { return delay_per_exec_; }

uint32_t Config::get_max_overall_mem() const { return max_overall_mem_; }
uint32_t Config::get_mem_per_frame() const { return mem_per_frame_; }
uint32_t Config::get_min_mem_per_proc() const { return min_mem_per_proc_; }
uint32_t Config::get_max_mem_per_proc() const { return max_mem_per_proc_; }

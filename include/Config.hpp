#pragma once

#include <string>
#include <cstdint>

class Config {
public:
  static Config &instance() {
    static Config instance;
    return instance;
  }
  bool load(const std::string &filename);
  uint8_t get_num_cpu() const;
  std::string get_scheduler() const;
  uint32_t get_quantum_cycles() const;
  uint32_t get_batch_process_freq() const;
  uint32_t get_min_ins() const;
  uint32_t get_max_ins() const;
  uint32_t get_delay_per_exec() const;

private:
  Config();
  ~Config() = default;

  // delete copy and move constructors and assign operators
  Config(const Config &) = delete;
  Config &operator=(const Config &) = delete;
  Config(Config &&) = delete;
  Config &operator=(Config &&) = delete;

  uint8_t num_cpu_;
  std::string scheduler_;
  uint32_t quantum_cycles_;
  uint32_t batch_process_freq_;
  uint32_t min_ins_;
  uint32_t max_ins_;
  uint32_t delay_per_exec_;
};

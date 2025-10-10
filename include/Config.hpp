#pragma once

#include <string>

class Config {
public:
  static Config &instance() {
    static Config instance;
    return instance;
  }
  bool load(const std::string &filename);
  int get_num_cpu() const;
  std::string get_scheduler() const;
  int get_quantum_cycles() const;
  int get_batch_process_freq() const;
  int get_min_ins() const;
  int get_max_ins() const;
  int get_delay_per_exec() const;

private:
  Config();
  ~Config() = default;

  // delete copy and move constructors and assign operators
  Config(const Config &) = delete;
  Config &operator=(const Config &) = delete;
  Config(Config &&) = delete;
  Config &operator=(Config &&) = delete;

  int num_cpu_;
  std::string scheduler_;
  int quantum_cycles_;
  int batch_process_freq_;
  int min_ins_;
  int max_ins_;
  int delay_per_exec_;
};

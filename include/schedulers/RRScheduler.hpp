#pragma once

#include "IScheduler.hpp"
#include <queue>

class RRScheduler : public IScheduler {
public:
  RRScheduler(int quantum_cycles);
  virtual ~RRScheduler() = default;
  void add_process(Process *process) override;
  Process *get_next_process() override;
  bool has_processes() const override;
  bool is_running() const override;
  void start() override;

private:
  std::priority_queue<QueuedProcess> ready_queue;
  bool is_running_ = false;
  int quantum_cycles;
};

#pragma once
#include "IScheduler.hpp"
#include "Process.hpp"
#include <queue>

class FCFSScheduler : public IScheduler {
public:
  virtual ~FCFSScheduler() = default;
  void add_process(Process *process) override;
  Process *get_next_process() override;
  bool has_processes() const override;
  bool is_running() const override;
  void start() override;
  void stop() override;

private:
  std::priority_queue<QueuedProcess> ready_queue;
  bool is_running_ = false;
};

#pragma once
#include "Process.hpp"

class IScheduler {
public:
  virtual ~IScheduler() = default;
  virtual void add_process(Process *process) = 0;
  virtual Process *get_next_process() = 0;
  virtual bool has_processes() const = 0;
};

struct QueuedProcess {
  Process *process;
  bool is_preempted;

  bool operator<(const QueuedProcess &other) const {
    return is_preempted < other.is_preempted;
  }
};

#pragma once

class IScheduler {
public:
  virtual ~IScheduler() = default;
  virtual void add_process(int pid) = 0;
  virtual int get_next_process() = 0;
  virtual bool has_processes() const = 0;
};

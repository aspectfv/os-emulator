#pragma once
#include "Process.hpp"

class CPUCore {
public:
  CPUCore(int id, const std::string scheduler, const int delay_per_exec);
  void tick();
  bool is_idle();
  const Process *get_current_process() const;
  void set_current_process(Process *process);

private:
  int id_;
  Process *current_process_ = nullptr;
  const std::string scheduler_ = "fcfs";
  const int delay_per_exec_;
};

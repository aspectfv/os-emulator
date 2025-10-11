#pragma once
#include "Process.hpp"

class CPUCore {
public:
  CPUCore(int id, const std::string &scheduler, const int delay_per_exec);
  void tick();

private:
  int id_;
  Process *current_process_ = nullptr;
  const std::string &scheduler_ = "fcfs";
  const int delay_per_exec_;
};

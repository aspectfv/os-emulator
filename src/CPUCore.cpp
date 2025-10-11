#include "CPUCore.hpp"

CPUCore::CPUCore(int id, const std::string scheduler, const int delay_per_exec)
    : id_(id), scheduler_(scheduler), delay_per_exec_(delay_per_exec) {}

void CPUCore::tick() {}

bool CPUCore::is_idle() { return current_process_ == nullptr; }

const Process *CPUCore::get_current_process() const { return current_process_; }

void CPUCore::set_current_process(Process *process) {
  current_process_ = process;
}

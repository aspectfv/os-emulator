#include "CPUCore.hpp"

CPUCore::CPUCore(int id, const std::string scheduler, const int delay_per_exec)
    : id_(id), scheduler_(scheduler), delay_per_exec_(delay_per_exec) {}

void CPUCore::tick() {
  current_process_->execute_current_instruction();

  if (current_process_->is_finished()) {
    current_process_->set_state(Process::ProcessState::TERMINATED);
    current_process_ = nullptr;
  }

  if (scheduler_ == "rr") {
    current_process_->decrement_quantum_remaining();

    if (current_process_->is_quantum_expired()) {
      current_process_->set_state(Process::ProcessState::READY);
      current_process_ = nullptr;
    }
  }
}

bool CPUCore::is_idle() { return current_process_ == nullptr; }

const Process *CPUCore::get_current_process() const { return current_process_; }

void CPUCore::set_current_process(Process *process) {
  current_process_ = process;
}

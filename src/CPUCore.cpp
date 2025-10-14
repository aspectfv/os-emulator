#include "CPUCore.hpp"
#include "Process.hpp"
#include <chrono>
#include <iostream>

CPUCore::CPUCore(int id, const std::string scheduler, const int delay_per_exec)
    : id_(id), scheduler_(scheduler), delay_per_exec_(delay_per_exec) {}

Process *CPUCore::tick() {
  busy_wait(delay_per_exec_);

  current_process_->execute_current_instruction(id_);
  current_process_->increment_instruction_pointer();

  if (current_process_->get_state() == Process::ProcessState::TERMINATED) {
    Process *finished_process = current_process_;
    current_process_ = nullptr;

    return finished_process;
  }

  if (current_process_->get_state() == Process::ProcessState::SLEEPING) {
    Process *waiting_process = current_process_;
    current_process_ = nullptr;

    return waiting_process;
  }

  if (scheduler_ == "rr") {
    current_process_->decrement_quantum_remaining();

    if (current_process_->is_quantum_expired()) {
      current_process_->set_state(Process::ProcessState::READY);

      Process *preempted_process = current_process_;
      current_process_ = nullptr;

      return preempted_process;
    }
  }

  return nullptr;
}

const bool CPUCore::is_idle() const { return current_process_ == nullptr; }

const Process *CPUCore::get_current_process() const { return current_process_; }

void CPUCore::set_current_process(Process *process) {
  current_process_ = process;

  if (process)
    current_process_->set_state(Process::ProcessState::RUNNING);
}

void CPUCore::busy_wait(int cycles) {
  if (cycles <= 0)
    return;

  auto start_time = std::chrono::high_resolution_clock::now();
  auto end_time =
      start_time + std::chrono::microseconds(cycles); // 1 cycle = 1 microsecond

  while (std::chrono::high_resolution_clock::now() < end_time)
    ;
}

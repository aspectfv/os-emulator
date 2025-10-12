#include "schedulers/RRScheduler.hpp"

RRScheduler::RRScheduler(int quantum_cycles) : quantum_cycles(quantum_cycles) {}

void RRScheduler::add_process(Process *process) {
  ready_queue.push(QueuedProcess{process, false});
}

void RRScheduler::add_process(Process *process, bool is_preempted) {
  ready_queue.push(QueuedProcess{process, is_preempted});
}

Process *RRScheduler::get_next_process() {
  if (ready_queue.empty()) {
    return nullptr;
  }

  QueuedProcess qp = ready_queue.top();
  ready_queue.pop();

  qp.process->set_quantum_remaining(quantum_cycles);

  return qp.process;
}

bool RRScheduler::has_processes() const { return !ready_queue.empty(); }

bool RRScheduler::is_running() const { return is_running_; }

void RRScheduler::start() { is_running_ = true; }

void RRScheduler::stop() { is_running_ = false; }

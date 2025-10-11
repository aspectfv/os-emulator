#include "schedulers/FCFSSCheduler.hpp"

void FCFSScheduler::add_process(Process *process) {
  ready_queue.push(QueuedProcess{process, false});
}

Process *FCFSScheduler::get_next_process() {
  if (ready_queue.empty()) {
    return nullptr;
  }

  QueuedProcess qp = ready_queue.top();
  ready_queue.pop();

  return qp.process;
}

bool FCFSScheduler::has_processes() const { return !ready_queue.empty(); }

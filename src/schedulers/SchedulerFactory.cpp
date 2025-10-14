#include "schedulers/SchedulerFactory.hpp"
#include "schedulers/FCFSScheduler.hpp"
#include "schedulers/RRScheduler.hpp"

std::unique_ptr<IScheduler>
SchedulerFactory::create_scheduler(const std::string &type, int quantum) {
  if (type == "rr") {
    return std::make_unique<RRScheduler>(quantum);
  } else if (type == "fcfs") {
    return std::make_unique<FCFSScheduler>();
  } else {
    throw std::invalid_argument("Unknown scheduler type: " + type);
  }
}

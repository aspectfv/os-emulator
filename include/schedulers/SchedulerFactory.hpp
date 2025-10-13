#pragma once
#include "schedulers/FCFSScheduler.hpp"
#include "schedulers/IScheduler.hpp"
#include "schedulers/RRScheduler.hpp"
#include <memory>
#include <string>

class SchedulerFactory {
public:
  static std::unique_ptr<IScheduler> createScheduler(const std::string &type,
                                                     int quantum = 0) {
    if (type == "rr") {
      return std::make_unique<RRScheduler>(quantum);
    } else if (type == "fcfs") {
      return std::make_unique<FCFSScheduler>();
    } else {
      throw std::invalid_argument("Unknown scheduler type: " + type);
    }
  }
};

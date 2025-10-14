#pragma once
#include "schedulers/IScheduler.hpp"
#include <memory>
#include <string>

class SchedulerFactory {
public:
  static std::unique_ptr<IScheduler> create_scheduler(const std::string &type,
                                                      int quantum = 0);
};

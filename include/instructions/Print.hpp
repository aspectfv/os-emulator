#pragma once
#include "IInstruction.hpp"
#include <string>

class Print : public IInstruction {
public:
  Print(const std::string &msg);
  void execute(Process *process, int cpu_core_id) override;
  const std::string &get_message() const;

private:
  std::string message_;
};

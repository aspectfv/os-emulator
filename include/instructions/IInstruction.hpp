#pragma once
#include <functional>
#include <string>

// forward declare to avoid circular dependency
class Process;

struct InstructionContext {
  std::string timestamp;
  std::function<bool(const std::string &)> variable_exists;
  std::function<void(const std::string &)> add_log;
};

class IInstruction {
public:
  virtual ~IInstruction() = default;
  virtual void execute(InstructionContext context) = 0;
};

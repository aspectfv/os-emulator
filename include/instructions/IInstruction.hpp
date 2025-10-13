#pragma once
#include <functional>
#include <optional>
#include <string>

// forward declare to avoid circular dependency
class Process;

struct InstructionContext {
  std::optional<std::function<void(const std::string &)>> add_log;
  std::optional<std::function<void(const std::string &, uint16_t)>>
      add_variable;
};

class IInstruction {
public:
  virtual ~IInstruction() = default;
  virtual void execute(InstructionContext context) = 0;
};

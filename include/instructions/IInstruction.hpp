#pragma once
#include <functional>
#include <optional>
#include <string>
#include <utility>

// forward declare to avoid circular dependency
class Process;

struct InstructionContext {
  std::optional<std::function<void(const std::string &)>> add_log;
  std::optional<std::function<void(const std::string &)>> get_variable;
  std::optional<std::function<void(std::pair<std::string, uint16_t>)>>
      add_variable;
  std::optional<std::function<void(uint8_t)>> sleep;
};

class IInstruction {
public:
  virtual ~IInstruction() = default;
  virtual void execute(InstructionContext context) = 0;
};

#pragma once
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// forward declare to avoid circular dependency
class Process;
class IInstruction;

struct InstructionContext {
  std::function<void(const std::string &)> add_log;
  std::function<uint16_t(const std::string &)> get_variable;
  std::function<void(std::pair<std::string, uint16_t>)> add_variable;
  std::function<void(std::vector<std::unique_ptr<IInstruction>> &&)>
      add_instructions;
  std::function<void(uint8_t)> sleep;
};

class IInstruction {
public:
  virtual ~IInstruction() = default;
  virtual void execute(InstructionContext context) = 0;
  virtual std::unique_ptr<IInstruction> clone() = 0;
};

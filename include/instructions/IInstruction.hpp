#pragma once
#include <functional>
#include <string>
#include <utility>

// forward declare to avoid circular dependency
class Process;
class IInstruction;

struct InstructionContext {
  std::function<void(const std::string &)> add_log = [](const std::string &) {};

  std::function<uint16_t(const std::string &)> get_variable =
      [](const std::string &) { return uint16_t{0}; };

  std::function<void(std::pair<std::string, uint16_t>)> add_variable =
      [](std::pair<std::string, uint16_t>) {};

  std::function<void(std::vector<std::unique_ptr<IInstruction>>)>
      add_instructions = [](std::vector<std::unique_ptr<IInstruction>>) {};

  std::function<void(uint8_t)> sleep = [](uint8_t) {};
};

class IInstruction {
public:
  virtual ~IInstruction() = default;
  virtual void execute(InstructionContext context) = 0;
};

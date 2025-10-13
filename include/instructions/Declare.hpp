#pragma once
#include "IInstruction.hpp"

class Declare : public IInstruction {
public:
  Declare(std::pair<std::string, uint16_t> variable);
  void execute(InstructionContext context) override;

private:
  std::pair<std::string, uint16_t> variable_;
};

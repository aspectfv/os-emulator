#pragma once
#include "IInstruction.hpp"

class Declare : public IInstruction {
public:
  using Variable = std::pair<std::string, uint16_t>;
  Declare(const Variable &variable);
  void execute(InstructionContext context) override;

private:
  Variable variable_;
};

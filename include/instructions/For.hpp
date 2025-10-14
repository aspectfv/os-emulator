#pragma once
#include "instructions/IInstruction.hpp"

class For : public IInstruction {
public:
  For(std::vector<IInstruction *> instructions, int repeats);
  void execute(InstructionContext context) override;

private:
  std::vector<IInstruction *> instructions_;
  int repeats_;
};

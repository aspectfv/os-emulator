#pragma once
#include "instructions/IInstruction.hpp"

class For : public IInstruction {
public:
  For(std::vector<std::unique_ptr<IInstruction>> instructions, int repeats);
  void execute(InstructionContext context) override;
  std::unique_ptr<IInstruction> clone() override;

private:
  std::vector<std::unique_ptr<IInstruction>> instructions_;
  int repeats_;
};

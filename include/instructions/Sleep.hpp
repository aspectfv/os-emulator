#pragma once
#include "instructions/IInstruction.hpp"

class Sleep : public IInstruction {
public:
  Sleep(uint8_t ticks);
  void execute(InstructionContext context) override;
  std::unique_ptr<IInstruction> clone() override;

private:
  uint8_t ticks_;
};

#pragma once
#include "instructions/IInstruction.hpp"

class Sleep : public IInstruction {
public:
  Sleep(uint8_t ticks);
  void execute(InstructionContext context) override;

private:
  uint8_t ticks_;
};

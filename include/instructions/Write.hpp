#pragma once

#include "instructions/IInstruction.hpp"
#include <memory>

class Write : public IInstruction {
public:
  Write(uint32_t memory_address, uint16_t value);
  void execute(InstructionContext context) override;
  std::unique_ptr<IInstruction> clone() override;

private:
  uint32_t memory_address_;
  uint16_t value_;
};

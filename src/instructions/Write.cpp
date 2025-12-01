#include "instructions/Write.hpp"

Write::Write(uint32_t memory_address, uint16_t value)
    : memory_address_(memory_address), value_(value) {}

void Write::execute(InstructionContext context) {
  context.write_to_address(memory_address_, value_);
}

std::unique_ptr<IInstruction> Write::clone() {
  return std::make_unique<Write>(memory_address_, value_);
}

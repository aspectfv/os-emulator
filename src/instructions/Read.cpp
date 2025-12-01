#include "instructions/Read.hpp"

Read::Read(const std::string &var_name, uint32_t memory_address)
    : var_name_(var_name), memory_address_(memory_address) {}

void Read::execute(InstructionContext context) {
  uint16_t value = context.read_from_address(memory_address_);
  context.add_variable({var_name_, value});
}

std::unique_ptr<IInstruction> Read::clone() {
  return std::make_unique<Read>(var_name_, memory_address_);
}

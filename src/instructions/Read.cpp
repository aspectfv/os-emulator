#include "instructions/Read.hpp"

Read::Read(const std::string &var, const std::string memory_address)
    : var_(var), memory_address_(memory_address) {}

void Read::execute(InstructionContext context) {
  uint16_t address = context.get_variable(memory_address_);
  uint16_t value = context.get_variable(var_);

  context.add_log("Read value " + std::to_string(value) +
                  " from memory address " + std::to_string(address));
}

std::unique_ptr<IInstruction> Read::clone() {
  return std::make_unique<Read>(var_, memory_address_);
}

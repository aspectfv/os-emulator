#include "instructions/Declare.hpp"

Declare::Declare(std::pair<std::string, uint16_t> variable)
    : variable_(variable) {}

void Declare::execute(InstructionContext context) {
  if (context.add_variable) {
    context.add_variable.value()(variable_);
  }
}

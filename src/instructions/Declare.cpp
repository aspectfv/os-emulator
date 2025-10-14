#include "instructions/Declare.hpp"

Declare::Declare(const Variable &variable) : variable_(variable) {}

void Declare::execute(InstructionContext context) {
  context.add_variable(variable_);
}

std::unique_ptr<IInstruction> Declare::clone() {
  return std::make_unique<Declare>(variable_);
}

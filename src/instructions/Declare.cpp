#include "instructions/Declare.hpp"

Declare::Declare(const Variable &variable) : variable_(variable) {}

void Declare::execute(InstructionContext context) {
  context.add_variable(variable_);
}

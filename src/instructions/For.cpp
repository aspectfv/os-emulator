#include "instructions/For.hpp"

For::For(const std::vector<IInstruction *> &instructions, int repeats)
    : instructions_(instructions), repeats_(repeats) {}

void For::execute(InstructionContext context) {
  for (int i = 0; i < repeats_; i++) {
    context.add_instructions(instructions_);
  }
}

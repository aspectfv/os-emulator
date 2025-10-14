#include "instructions/For.hpp"

For::For(std::vector<std::unique_ptr<IInstruction>> instructions, int repeats)
    : instructions_(std::move(instructions)), repeats_(repeats) {}

void For::execute(InstructionContext context) {
  static thread_local int depth = 0;

  if (depth >= 3) {
    return;
  }

  ++depth;

  for (int i = 0; i < repeats_; ++i) {
    context.add_instructions(std::move(instructions_));
  }

  --depth;
}

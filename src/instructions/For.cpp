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
    std::vector<std::unique_ptr<IInstruction>> cloned_instructions;

    for (const auto &instr : instructions_) {
      cloned_instructions.push_back(instr->clone());
    }

    context.add_instructions(std::move(cloned_instructions));
  }

  --depth;
}

std::unique_ptr<IInstruction> For::clone() {
  std::vector<std::unique_ptr<IInstruction>> cloned_instructions;
  for (const auto &instr : instructions_) {
    cloned_instructions.push_back(instr->clone());
  }
  return std::make_unique<For>(std::move(cloned_instructions), repeats_);
}

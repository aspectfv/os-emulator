#include "instructions/For.hpp"

For::For(std::vector<std::unique_ptr<IInstruction>> instructions, int repeats)
    : instructions_(std::move(instructions)), repeats_(repeats) {}

void For::execute(InstructionContext context) {
  static thread_local int depth = 0;

  if (depth >= 3) {
    return;
  }

  ++depth;

  context.add_log("Entering For loop with " + std::to_string(repeats_) +
                  " repeats at depth " + std::to_string(depth));

  for (int i = 0; i < repeats_; ++i) {
    std::vector<std::unique_ptr<IInstruction>> cloned_instructions;

    context.add_log("For loop iteration " + std::to_string(i + 1));

    for (const auto &instr : instructions_) {
      cloned_instructions.push_back(instr->clone());

      // suppress warning hack
      const IInstruction &instr_ref = *instr;
      std::string instruction_type = typeid(instr_ref).name();

      instruction_type.erase(
          std::remove_if(instruction_type.begin(), instruction_type.end(),
                         [](unsigned char c) { return std::isdigit(c); }),
          instruction_type.end());

      context.add_log("Instruction Type: " + instruction_type);
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

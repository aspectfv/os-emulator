#include "Process.hpp"

void Process::execute_current_instruction() {
  if (instruction_pointer_ < total_instructions_) {
    instructions_[instruction_pointer_]->execute();
    instruction_pointer_++;
  }
}

void Process::set_instructions(
    std::vector<std::unique_ptr<IInstruction>> &&instructions) {
  instructions_ = std::move(instructions);
  total_instructions_ = instructions_.size();
  instruction_pointer_ = 0;
}

void Process::set_quantum_remaining(int quantum_cycles) {
  quantum_remaining_ = quantum_cycles;
}

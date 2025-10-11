#include "Process.hpp"

void Process::execute_current_instruction() {
  if (instruction_pointer_ < total_instructions_) {
    instructions_[instruction_pointer_]->execute();
    instruction_pointer_++;
  }
}

#include "Process.hpp"

Process::Process(int id, const std::string &name, const std::string &created_at)
    : id_(id), name_(name), created_at_(created_at), instruction_pointer_(0),
      total_instructions_(0), state_(ProcessState::NEW), quantum_remaining_(0) {
}

void Process::execute_current_instruction() {
  if (instruction_pointer_ < total_instructions_) {
    instructions_[instruction_pointer_]->execute();
    instruction_pointer_++;
  }
}

bool Process::is_finished() const {
  return instruction_pointer_ >= total_instructions_;
}

int Process::is_quantum_expired() const { return quantum_remaining_ <= 0; }

void Process::set_instructions(
    std::vector<std::unique_ptr<IInstruction>> &&instructions) {
  instructions_ = std::move(instructions);
  total_instructions_ = instructions_.size();
  instruction_pointer_ = 0;
}

void Process::set_state(ProcessState state) { state_ = state; }

void Process::set_quantum_remaining(int quantum_cycles) {
  quantum_remaining_ = quantum_cycles;
}

void Process::decrement_quantum_remaining() { quantum_remaining_--; }

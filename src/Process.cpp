#include "Process.hpp"
#include <iostream>

// auto inc process id
int Process::next_id_ = 0;

Process::Process(const std::string &name, const std::string &created_at,
                 int total_instructions, int quantum_cycles)
    : id_(next_id_++), name_(name), created_at_(created_at),
      total_instructions_(total_instructions), instruction_pointer_(0),
      state_(ProcessState::NEW), quantum_remaining_(quantum_cycles) {}

void Process::execute_current_instruction(int cpu_core_id) {
  if (instruction_pointer_ < total_instructions_) {
    instructions_[instruction_pointer_]->execute(
        {.add_log =
             [this, cpu_core_id](const std::string &message) {
               this->logs_.push_back(ProcessLog{.timestamp = "",
                                                .core_id = cpu_core_id,
                                                .message = message});
             },
         .get_variable =
             [this](const std::string &var_name) {
               return this->symbol_table_[var_name];
             },
         .add_variable =
             [this](std::pair<std::string, uint16_t> var) {
               this->symbol_table_.insert(var);
             },
         .add_instructions =
             [this](std::vector<std::unique_ptr<IInstruction>>
                        &&new_instructions) {
               this->instructions_.insert(
                   this->instructions_.end(),
                   std::make_move_iterator(new_instructions.begin()),
                   std::make_move_iterator(new_instructions.end()));
               this->total_instructions_ = this->instructions_.size();
             },
         .sleep =
             [this](uint8_t cycles) { this->quantum_remaining_ -= cycles; }});
  }
}

const int Process::get_id() const { return id_; }

const std::string Process::get_name() const { return name_; }

const std::string Process::get_created_at() const { return created_at_; }

const int Process::get_total_instructions() const {
  return total_instructions_;
}

const int Process::get_instruction_pointer() const {
  return instruction_pointer_;
}

const Process::ProcessState Process::get_state() const { return state_; }

const bool Process::is_finished() const {
  return instruction_pointer_ >= total_instructions_;
}

const int Process::is_quantum_expired() const {
  return quantum_remaining_ <= 0;
}

void Process::increment_instruction_pointer() { instruction_pointer_++; }

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

void Process::print_instructions() const {
  for (size_t i = 0; i < instructions_.size(); ++i) {
    std::string type_name = typeid(*instructions_[i]).name();

    // Remove leading digits
    size_t pos = 0;
    while (pos < type_name.size() && std::isdigit(type_name[pos])) {
      ++pos;
    }
    type_name = type_name.substr(pos);

    std::cout << i << ": " << type_name << "\n";
  }

  std::cout << std::endl;
  std::cout << total_instructions_ << std::endl;
}

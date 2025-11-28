#include "Process.hpp"

// auto inc process id
int Process::next_id_ = 0;

Process::Process(const std::string &name, int total_instructions,
                 int quantum_cycles)
    : id_(next_id_++), name_(name), total_instructions_(total_instructions),
      state_(ProcessState::NEW), quantum_remaining_(quantum_cycles) {
  symbol_table_["x"] = 0; // required for mo1 demo
}

void Process::execute_current_instruction(int cpu_core_id) {
  if (instruction_pointer_ < instructions_.size()) {
    instructions_[instruction_pointer_]->execute(
        {.add_log =
             [this, cpu_core_id](const std::string &message) {
               this->add_log(cpu_core_id, message);
             },
         .get_variable =
             [this](const std::string &var_name) {
               return this->get_variable(var_name);
             },
         .add_variable =
             [this](std::pair<std::string, uint16_t> var) {
               this->symbol_table_[var.first] = var.second;
             },
         .add_instructions =
             [this](std::vector<std::unique_ptr<IInstruction>>
                        &&new_instructions) {
               this->add_instructions(std::move(new_instructions));
             },

         .sleep = [this](int ticks) { this->sleep(ticks); }});
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

const int Process::is_quantum_expired() const {
  return quantum_remaining_ <= 0;
}

const int Process::get_page_table_size() const { return page_table_.size(); }

const PageTableEntry &
Process::get_page_table_entry(int virtual_page_number) const {
  return page_table_.at(virtual_page_number);
}

PageTableEntry &Process::get_page_table_entry(int virtual_page_number) {
  return page_table_.at(virtual_page_number);
}

const uint32_t Process::get_total_memory_size() const {
  return total_memory_size_;
}

const uint32_t Process::get_backing_store_offset() const {
  return backing_store_offset_;
}

const bool Process::get_access_violation() const { return access_violation_; }

void Process::increment_instruction_pointer() {
  if (instruction_pointer_ < instructions_.size())
    instruction_pointer_++;
  if (instruction_pointer_ >= instructions_.size()) {
    state_ = ProcessState::TERMINATED;
    total_instructions_ = instructions_.size();
  }
}

void Process::set_instructions(
    std::vector<std::unique_ptr<IInstruction>> &&instructions) {
  instructions_ = std::move(instructions);
}

void Process::set_state(ProcessState state) { state_ = state; }

void Process::set_quantum_remaining(int quantum_cycles) {
  quantum_remaining_ = quantum_cycles;
}

void Process::decrement_quantum_remaining() { quantum_remaining_--; }

void Process::decrement_sleep_ticks() {
  if (sleep_ticks_ > 0)
    sleep_ticks_--;
  else
    state_ = ProcessState::READY;
}

void Process::set_total_memory_size(uint32_t size) {
  total_memory_size_ = size;
}

void Process::set_backing_store_offset(uint32_t offset) {
  backing_store_offset_ = offset;
}
void Process::set_access_violation(bool violation) {
  access_violation_ = violation;
}

void Process::add_log(int cpu_core_id, const std::string &message) {
  logs_.push_back(ProcessLog{.core_id = cpu_core_id, .message = message});
}

uint16_t Process::get_variable(const std::string &var_name) {
  return symbol_table_[var_name];
}

void Process::add_instructions(
    std::vector<std::unique_ptr<IInstruction>> &&new_instructions) {
  auto insert_pos = instructions_.begin() + instruction_pointer_ + 1;

  instructions_.insert(insert_pos,
                       std::make_move_iterator(new_instructions.begin()),
                       std::make_move_iterator(new_instructions.end()));
}

void Process::sleep(int ticks) {
  sleep_ticks_ = ticks;
  state_ = ProcessState::SLEEPING;
}

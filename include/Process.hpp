#pragma once
#include "instructions/IInstruction.hpp"
#include <memory>
#include <string>
#include <vector>

class Process {
public:
  enum class ProcessState { NEW, READY, RUNNING, WAITING, TERMINATED };

  Process(int id, const std::string &name, const std::string &created_at);
  void execute_current_instruction();
  bool is_finished() const;
  int is_quantum_expired() const;
  void
  set_instructions(std::vector<std::unique_ptr<IInstruction>> &&instructions);
  void set_state(ProcessState state);
  void set_quantum_remaining(int quantum_cycles);
  void decrement_quantum_remaining();

private:
  int id_;
  std::string name_;
  std::string created_at_;
  std::vector<std::unique_ptr<IInstruction>> instructions_;
  int total_instructions_;
  int instruction_pointer_;
  ProcessState state_;
  int quantum_remaining_;
};

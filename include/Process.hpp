#pragma once
#include "instructions/IInstruction.hpp"
#include <memory>
#include <string>
#include <vector>

class Process {
public:
  Process(int id, const std::string &name, const std::string &created_at);
  void execute_current_instruction();
  void
  set_instructions(std::vector<std::unique_ptr<IInstruction>> &&instructions);
  void set_quantum_remaining(int quantum_cycles);

private:
  int id_;
  std::string name_;
  std::string created_at_;
  std::vector<std::unique_ptr<IInstruction>> instructions_;
  int total_instructions_;
  int instruction_pointer_;
  enum class State { NEW, READY, RUNNING, WAITING, TERMINATED } state_;
  int quantum_remaining_;
};

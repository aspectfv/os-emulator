#pragma once
#include "instructions/IInstruction.hpp"
#include <string>
#include <vector>

class Process {
public:
private:
  int id_;
  std::string name_;
  std::string created_at_;
  std::vector<IInstruction> instructions_;
  int total_instructions_;
  int instruction_pointer_;
  enum class State { NEW, READY, RUNNING, WAITING, TERMINATED } state_;
};

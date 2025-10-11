#pragma once
#include "instructions/IInstruction.hpp"
#include <memory>
#include <string>
#include <vector>

class Process {
public:
private:
  int id_;
  std::string name_;
  std::string created_at_;
  std::vector<std::unique_ptr<IInstruction>> instructions_;
  int total_instructions_;
  int instruction_pointer_;
  enum class State { NEW, READY, RUNNING, WAITING, TERMINATED } state_;
};

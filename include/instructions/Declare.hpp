#pragma once
#include "IInstruction.hpp"

class Declare : public IInstruction {
public:
  Declare(const std::string &var_name, uint16_t value);
  void execute(InstructionContext context) override;

private:
  std::string var_name_;
  uint16_t value_;
};

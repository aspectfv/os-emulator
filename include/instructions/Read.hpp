#pragma once

#include "instructions/IInstruction.hpp"

class Read : public IInstruction {
public:
  Read(const std::string &var_name, uint32_t memory_address);
  void execute(InstructionContext context) override;
  std::unique_ptr<IInstruction> clone() override;

private:
  std::string var_name_;
  uint32_t memory_address_;
};

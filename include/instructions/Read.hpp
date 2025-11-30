#pragma once

#include "instructions/IInstruction.hpp"

class Read : public IInstruction {
public:
  Read(const std::string &var, const std::string memory_address);
  void execute(InstructionContext context) override;
  std::unique_ptr<IInstruction> clone() override;

private:
  std::string var_;
  std::string memory_address_;
};

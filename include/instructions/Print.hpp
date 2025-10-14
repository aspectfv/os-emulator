#pragma once
#include "IInstruction.hpp"

class Print : public IInstruction {
public:
  Print(const std::string &msg);
  void execute(InstructionContext context) override;
  std::unique_ptr<IInstruction> clone() override;

private:
  std::string message_;
};

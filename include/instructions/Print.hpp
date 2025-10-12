#pragma once
#include "IInstruction.hpp"
#include <string>

class Print : public IInstruction {
public:
  Print(const std::string &msg);
  void execute(InstructionContext context) override;
  const std::string &get_message() const;

private:
  std::string message_;
};

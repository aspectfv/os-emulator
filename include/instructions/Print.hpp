#pragma once
#include "IInstruction.hpp"
#include <string>

class Print : public IInstruction {
public:
  Print(const std::string &msg);
  void execute() override;

private:
  std::string message;
};

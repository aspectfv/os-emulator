#pragma once
#include "IInstruction.hpp"
#include <iostream>
#include <string>

class Print : public IInstruction {
public:
  void execute() override { std::cout << message << std::endl; }

private:
  std::string message;
};

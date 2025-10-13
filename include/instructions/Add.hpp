#pragma once
#include "instructions/IInstruction.hpp"
#include <string>
#include <utility>

class Add : public IInstruction {
public:
  Add(std::pair<std::string, uint16_t> var1,
      std::variant<std::pair<std::string, uint16_t>, uint16_t> var2,
      std::variant<std::pair<std::string, uint16_t>, uint16_t> var3);
  void execute(InstructionContext context) override;

private:
  std::pair<std::string, uint16_t> var1_;
  std::pair<std::string, uint16_t> var2_;
  std::pair<std::string, uint16_t> var3_;
};

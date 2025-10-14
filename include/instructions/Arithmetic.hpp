#pragma once
#include "instructions/IInstruction.hpp"
#include <cstdint>
#include <string>
#include <variant>

class Arithmetic : public IInstruction {
public:
  enum class Operator { ADD, SUBTRACT };
  using Operand = std::variant<std::string, uint16_t>;

  Arithmetic(const std::string &var1, const Operand &var2, const Operand &var3,
             const Operator &op);
  void execute(InstructionContext context) override;

private:
  std::string var1_;
  Operand var2_;
  Operand var3_;
  Operator operator_;
};

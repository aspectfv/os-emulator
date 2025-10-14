#include "instructions/Arithmetic.hpp"

Arithmetic::Arithmetic(const std::string &var1, const Operand &var2,
                       const Operand &var3, const Operator &op)
    : var1_(var1), var2_(var2), var3_(var3), operator_(op) {}

void Arithmetic::execute(InstructionContext context) {
  uint16_t a = std::holds_alternative<std::string>(var2_)
                   ? context.get_variable(std::get<std::string>(var2_))
                   : std::get<uint16_t>(var2_);

  uint16_t b = std::holds_alternative<std::string>(var3_)
                   ? context.get_variable(std::get<std::string>(var3_))
                   : std::get<uint16_t>(var3_);

  uint16_t result = 0;
  switch (operator_) {
    case Operator::ADD:
      result = a + b;
      break;
    case Operator::SUB:
      result = a - b;
      break;
  }

  context.add_variable({var1_, result});
}

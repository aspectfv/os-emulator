#include "instructions/Arithmetic.hpp"
#include <algorithm>
#include <limits>

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
    case Operator::ADD: {
      int a_val = static_cast<int>(a);
      int b_val = static_cast<int>(b);
      result = std::clamp<int>(a_val + b_val, 0,
                               std::numeric_limits<uint16_t>::max());
      break;
    }
    case Operator::SUBTRACT: {
      int a_val = static_cast<int>(a);
      int b_val = static_cast<int>(b);
      result = std::clamp<int>(a_val - b_val, 0,
                               std::numeric_limits<uint16_t>::max());
      break;
    }
  }

  context.add_variable({var1_, result});
  // context.add_log("Arithmetic: " + var1_ + " = " + std::to_string(result));
}

std::unique_ptr<IInstruction> Arithmetic::clone() {
  return std::make_unique<Arithmetic>(var1_, var2_, var3_, operator_);
}

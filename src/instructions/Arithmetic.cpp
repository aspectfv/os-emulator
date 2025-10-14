#include "instructions/Arithmetic.hpp"

Arithmetic::Arithmetic(
    const std::pair<std::string, uint16_t> &var1,
    const std::variant<std::pair<std::string, uint16_t>, uint16_t> &var2,
    const std::variant<std::pair<std::string, uint16_t>, uint16_t> &var3)
    : var1_(var1), var2_(var2), var3_(var3) {}

void Arithmetic::execute(InstructionContext context) {
  if (context.get_variable && context.add_variable) {
    uint16_t value2 = 0;
    uint16_t value3 = 0;

    if (std::holds_alternative<std::pair<std::string, uint16_t>>(var2_)) {
      auto var2 = std::get<std::pair<std::string, uint16_t>>(var2_);
      value2 = context.get_variable(var2.first);
    } else {
      value2 = std::get<uint16_t>(var2_);
      context.add_variable(std::make_pair(var1_.first, value2));
    }

    if (std::holds_alternative<std::pair<std::string, uint16_t>>(var3_)) {
      auto var3 = std::get<std::pair<std::string, uint16_t>>(var3_);
      value3 = context.get_variable(var3.first);
    } else {
      value3 = std::get<uint16_t>(var3_);
      context.add_variable(std::make_pair(var1_.first, value3));
    }

    switch (operator_) {
      case Operator::ADD:
        context.add_variable(std::make_pair(var1_.first, value2 + value3));
        break;
      case Operator::SUB:
        context.add_variable(std::make_pair(var1_.first, value2 - value3));
        break;
      default:
        if (context.add_log) {
          context.add_log("Unsupported operator in Arithmetic");
        }
        break;
    }
  }
}

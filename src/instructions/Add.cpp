#include "instructions/Add.hpp"

Add::Add(const std::pair<std::string, uint16_t> &var1,
         const std::variant<std::pair<std::string, uint16_t>, uint16_t> &var2,
         const std::variant<std::pair<std::string, uint16_t>, uint16_t> &var3)
    : var1_(var1), var2_(var2), var3_(var3) {}

void Add::execute(InstructionContext context) {
  if (context.get_variable && context.add_variable) {
    uint16_t value2 = 0;
    uint16_t value3 = 0;

    if (std::holds_alternative<std::pair<std::string, uint16_t>>(var2_)) {
      auto var2 = std::get<std::pair<std::string, uint16_t>>(var2_);
      value2 = context.get_variable.value()(var2.first);
    } else {
      value2 = std::get<uint16_t>(var2_);
      context.add_variable.value()(std::make_pair(var1_.first, value2));
    }

    if (std::holds_alternative<std::pair<std::string, uint16_t>>(var3_)) {
      auto var3 = std::get<std::pair<std::string, uint16_t>>(var3_);
      value3 = context.get_variable.value()(var3.first);
    } else {
      value3 = std::get<uint16_t>(var3_);
      context.add_variable.value()(std::make_pair(var1_.first, value3));
    }

    uint16_t result = value2 + value3;
    context.add_variable.value()(std::make_pair(var1_.first, result));
  }
}

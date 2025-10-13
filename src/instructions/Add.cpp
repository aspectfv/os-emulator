#include "instructions/Add.hpp"

Add::Add(std::pair<std::string, uint16_t> var1,
         std::variant<std::pair<std::string, uint16_t>, uint16_t> var2,
         std::variant<std::pair<std::string, uint16_t>, uint16_t> var3)
    : var1_(std::move(var1)) {
  if (std::holds_alternative<std::pair<std::string, uint16_t>>(var2)) {
    var2_ = std::get<std::pair<std::string, uint16_t>>(var2);
  } else {
    var2_ = {"", std::get<uint16_t>(var2)};
  }

  if (std::holds_alternative<std::pair<std::string, uint16_t>>(var3)) {
    var3_ = std::get<std::pair<std::string, uint16_t>>(var3);
  } else {
    var3_ = {"", std::get<uint16_t>(var3)};
  }
}

void Add::execute(InstructionContext context) {
  uint16_t value2 = var2_.first.empty() ? var2_.second : 0;
  uint16_t value3 = var3_.first.empty() ? var3_.second : 0;

  if (!var2_.first.empty() && context.add_variable) {
    // Here we would normally look up the variable's value from some storage.
    // For this example, let's assume it's zero.
    value2 = 0; // Replace with actual lookup
  }

  if (!var3_.first.empty() && context.add_variable) {
    // Here we would normally look up the variable's value from some storage.
    // For this example, let's assume it's zero.
    value3 = 0; // Replace with actual lookup
  }

  uint16_t result = value2 + value3;

  if (context.add_variable) {
    context.add_variable->operator()(std::make_pair(var1_.first, result));
  }

  if (context.add_log) {
    context.add_log->operator()("Added " + std::to_string(value2) + " and " +
                                std::to_string(value3) + ", result stored in " +
                                var1_.first);
  }
}

#include "instructions/Declare.hpp"

Declare::Declare(const std::string &var_name, uint16_t value)
    : var_name_(var_name), value_(value) {}

void Declare::execute(InstructionContext context) {
  if (context.add_variable) {
    context.add_variable.value()(var_name_, value_);
  }
}

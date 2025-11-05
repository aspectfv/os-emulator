#include "instructions/Print.hpp"

Print::Print(const std::string &msg) : message_(msg) {
  size_t first_quote = msg.find('\"');

  if (first_quote != std::string::npos) {
    size_t second_quote = msg.find('\"', first_quote + 1);

    if (second_quote != std::string::npos) {
      prefix_ = msg.substr(0, first_quote);
      var_name_ = msg.substr(first_quote + 1, second_quote - first_quote - 1);
      suffix_ = msg.substr(second_quote + 1);
      return;
    }
  }

  prefix_ = msg;
  var_name_ = "";
  suffix_ = "";
}

void Print::execute(InstructionContext context) {
  if (!var_name_.empty()) {
    uint16_t value = context.get_variable(var_name_);
    context.add_log(prefix_ + std::to_string(value) + suffix_);
  } else {
    context.add_log(prefix_);
  }
}

std::unique_ptr<IInstruction> Print::clone() {
  return std::make_unique<Print>(message_);
}

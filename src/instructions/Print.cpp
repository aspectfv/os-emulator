#include "instructions/Print.hpp"

Print::Print(const std::string &msg) : message_(msg) {}

void Print::execute(InstructionContext context) {
  if (context.add_log) {
    context.add_log.value()(message_);
  }
}

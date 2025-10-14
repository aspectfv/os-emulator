#include "instructions/Print.hpp"

Print::Print(const std::string &msg) : message_(msg) {}

void Print::execute(InstructionContext context) { context.add_log(message_); }

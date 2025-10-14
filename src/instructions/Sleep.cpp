#include "instructions/Sleep.hpp"

Sleep::Sleep(uint8_t ticks) : ticks_(ticks) {}

void Sleep::execute(InstructionContext context) {
  context.sleep(ticks_);
  context.add_log("Sleep for " + std::to_string(ticks_) + " ticks");
}

std::unique_ptr<IInstruction> Sleep::clone() {
  return std::make_unique<Sleep>(ticks_);
}

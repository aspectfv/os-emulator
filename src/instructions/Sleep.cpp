#include "instructions/Sleep.hpp"

Sleep::Sleep(uint8_t ticks) : ticks_(ticks) {}

void Sleep::execute(InstructionContext context) {
  if (context.sleep) {
    context.sleep.value()(ticks_);
  }
}

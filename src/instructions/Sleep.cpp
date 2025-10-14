#include "instructions/Sleep.hpp"

Sleep::Sleep(uint8_t ticks) : ticks_(ticks) {}

void Sleep::execute(InstructionContext context) { context.sleep(ticks_); }

#include "CPUCore.hpp"

CPUCore::CPUCore(int id, const std::string &scheduler, const int delay_per_exec)
    : id_(id), scheduler_(scheduler), delay_per_exec_(delay_per_exec) {}

void CPUCore::tick() {}

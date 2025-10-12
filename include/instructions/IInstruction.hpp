#pragma once

// forward declare to avoid circular dependency
class Process;

class IInstruction {
public:
  virtual ~IInstruction() = default;
  virtual void execute(Process *process, int cpu_core_id) = 0;
};

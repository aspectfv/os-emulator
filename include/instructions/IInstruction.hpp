#pragma once

class IInstruction {
public:
  virtual ~IInstruction() = default;
  virtual void execute() = 0;
};

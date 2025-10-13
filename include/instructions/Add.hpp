#pragma once
#include "instructions/IInstruction.hpp"
#include <cstdint>
#include <string>
#include <utility>
#include <variant>

class Add : public IInstruction {
public:
  Add(const std::pair<std::string, uint16_t> &var1,
      const std::variant<std::pair<std::string, uint16_t>, uint16_t> &var2,
      const std::variant<std::pair<std::string, uint16_t>, uint16_t> &var3);
  void execute(InstructionContext context) override;

private:
  std::pair<std::string, uint16_t> var1_;
  std::variant<std::pair<std::string, uint16_t>, uint16_t> var2_;
  std::variant<std::pair<std::string, uint16_t>, uint16_t> var3_;
};

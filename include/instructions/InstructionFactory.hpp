#pragma once
#include "instructions/Arithmetic.hpp"
#include "instructions/Declare.hpp"
#include "instructions/IInstruction.hpp"
#include "instructions/Print.hpp"
#include "instructions/Sleep.hpp"
#include <cstdlib>
#include <ctime>
#include <memory>
#include <vector>

class InstructionFactory {
public:
  enum class InstructionType { PRINT, DECLARE, ADD, SUBTRACT, SLEEP, FOR };
  static std::vector<std::unique_ptr<IInstruction>>
  create_instructions(const std::string &process_name, int min_ins,
                      int max_ins);

private:
  static std::unique_ptr<Print> create_print(const std::string &msg);
  static std::unique_ptr<Declare> create_declare(const std::string &var_name,
                                                 uint16_t value);
  static std::unique_ptr<Arithmetic>
  create_arithmetic(const std::string &var1, const Arithmetic::Operand &var2,
                    const Arithmetic::Operand &var3, Arithmetic::Operator op);
  static std::unique_ptr<Sleep> create_sleep(uint8_t ticks);

  // helpers
  static Arithmetic::Operand random_operand();
};

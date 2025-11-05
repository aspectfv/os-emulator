#pragma once
#include "instructions/Arithmetic.hpp"
#include "instructions/Declare.hpp"
#include "instructions/For.hpp"
#include "instructions/IInstruction.hpp"
#include "instructions/Print.hpp"
#include "instructions/Sleep.hpp"
#include <memory>
#include <string>
#include <vector>

class InstructionFactory {
public:
  enum class InstructionType { PRINT, DECLARE, ADD, SUBTRACT, SLEEP, FOR };
  static std::vector<std::unique_ptr<IInstruction>>
  create_instructions(const std::string &process_name, int num_instructions,
                      int max_ins, int min_ins, int start_depth = 0,
                      int max_depth = 3);
  static std::vector<std::unique_ptr<IInstruction>>
  create_mo1_demo_instructions(const std::string &process_name,
                               int num_instructions);

private:
  static std::unique_ptr<Print> create_print(const std::string &msg);
  static std::unique_ptr<Declare> create_declare(const std::string &var_name,
                                                 uint16_t value);
  static std::unique_ptr<Arithmetic>
  create_arithmetic(const std::string &var1, const Arithmetic::Operand &var2,
                    const Arithmetic::Operand &var3, Arithmetic::Operator op);
  static std::unique_ptr<Sleep> create_sleep(uint8_t ticks);
  static std::unique_ptr<For>
  create_for(const std::string &process_name, int num_instructions, int min_ins,
             int max_ins, int repeats, int start_depth = 0, int max_depth = 3);

  // helpers
  static Arithmetic::Operand random_operand();
};

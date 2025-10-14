#include "instructions/InstructionFactory.hpp"
#include "instructions/Arithmetic.hpp"
#include "instructions/Declare.hpp"
#include "instructions/Print.hpp"
#include "instructions/Sleep.hpp"
#include <cstdlib>
#include <ctime>

std::vector<std::unique_ptr<IInstruction>>
InstructionFactory::create_instructions(const std::string &process_name,
                                        int min_ins, int max_ins) {
  std::vector<std::unique_ptr<IInstruction>> instructions;
  int num_instructions = rand() % (max_ins - min_ins + 1) + min_ins;

  // ensure at least 2 declare instructions for arithmetic operations
  instructions.push_back(create_declare("add_var", rand() % 100));
  instructions.push_back(create_declare("sub_var", rand() % 100));

  for (int i = 2; i < num_instructions; ++i) {
    int instruction_type = rand() % static_cast<int>(InstructionType::FOR);

    switch (static_cast<InstructionType>(instruction_type)) {
      case InstructionType::PRINT:
        instructions.push_back(
            create_print("Hello from process " + process_name));
        break;
      case InstructionType::DECLARE:
        instructions.push_back(
            create_declare("var" + std::to_string(i), rand() % 100));
        break;
      case InstructionType::ADD:
        instructions.push_back(create_arithmetic("add_var", random_operand(),
                                                 random_operand(),
                                                 Arithmetic::Operator::ADD));
        break;
      case InstructionType::SUBTRACT:
        instructions.push_back(
            create_arithmetic("sub_var", random_operand(), random_operand(),
                              Arithmetic::Operator::SUBTRACT));
        break;
      case InstructionType::SLEEP:
        instructions.push_back(
            create_sleep(rand() % 5 + 1)); // sleep between 1 and 5 ticks
        break;
      case InstructionType::FOR:
        // add logic
        break;
      default:
        break;
    }
  }

  return instructions;
}

std::unique_ptr<Print>
InstructionFactory::create_print(const std::string &msg) {
  return std::make_unique<Print>(msg);
}

std::unique_ptr<Declare>
InstructionFactory::create_declare(const std::string &var_name,
                                   uint16_t value) {
  return std::make_unique<Declare>(Declare::Variable(var_name, value));
}

std::unique_ptr<Arithmetic> InstructionFactory::create_arithmetic(
    const std::string &var1, const Arithmetic::Operand &var2,
    const Arithmetic::Operand &var3, Arithmetic::Operator op) {
  return std::make_unique<Arithmetic>(var1, var2, var3, op);
}

std::unique_ptr<Sleep> InstructionFactory::create_sleep(uint8_t ticks) {
  return std::make_unique<Sleep>(ticks);
}

// helpers
Arithmetic::Operand InstructionFactory::random_operand() {
  int choice = rand() % 2;

  switch (choice) {
    case 0: // Variable
      return Arithmetic::Operand("var" + std::to_string(rand() % 100));
    case 1: // Constant
      return Arithmetic::Operand(uint16_t(rand() % 100));
    default:
      return Arithmetic::Operand(uint16_t(0));
  }
}

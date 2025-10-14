#include "instructions/InstructionFactory.hpp"
#include "instructions/Arithmetic.hpp"
#include "instructions/Declare.hpp"
#include "instructions/Print.hpp"
#include "instructions/Sleep.hpp"

std::vector<std::unique_ptr<IInstruction>>
InstructionFactory::create_instructions(const std::string &process_name,
                                        int num_instructions, int max_ins,
                                        int min_ins, int start_depth,
                                        int max_depth) {
  if (start_depth >= max_depth) {
    return {};
  }

  std::vector<std::unique_ptr<IInstruction>> instructions;

  for (int i = 0; i < num_instructions; ++i) {
    int instruction_type =
        rand() %
        (static_cast<int>(InstructionFactory::InstructionType::FOR) + 1);

    switch (
        static_cast<InstructionFactory::InstructionType>(instruction_type)) {
      case InstructionFactory::InstructionType::PRINT:
        instructions.push_back(InstructionFactory::create_print(
            "Hello from process " + process_name));
        break;
      case InstructionFactory::InstructionType::DECLARE:
        instructions.push_back(InstructionFactory::create_declare(
            "var" + std::to_string(i), rand() % 100));
        break;
      case InstructionFactory::InstructionType::ADD:
        instructions.push_back(InstructionFactory::create_arithmetic(
            "add_var", InstructionFactory::random_operand(),
            InstructionFactory::random_operand(), Arithmetic::Operator::ADD));
        break;
      case InstructionFactory::InstructionType::SUBTRACT:
        instructions.push_back(InstructionFactory::create_arithmetic(
            "sub_var", InstructionFactory::random_operand(),
            InstructionFactory::random_operand(),
            Arithmetic::Operator::SUBTRACT));
        break;
      case InstructionFactory::InstructionType::SLEEP:
        instructions.push_back(InstructionFactory::create_sleep(
            rand() % 5 + 1)); // sleep between 1 and 5 ticks
        break;
      case InstructionFactory::InstructionType::FOR: {
        int num_instructions = min_ins + (rand() % (max_ins - min_ins + 1));
        int repeats = rand() % 3 + 1; // repeat between 1 and 3 times

        instructions.push_back(InstructionFactory::create_for(
            process_name, num_instructions, repeats, start_depth, max_depth));
        break;
      }
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

std::unique_ptr<For>
InstructionFactory::create_for(const std::string &process_name,
                               int num_instructions, int min_ins, int max_ins,
                               int repeats, int start_depth, int max_depth) {
  return std::make_unique<For>(create_instructions(process_name, min_ins,
                                                   max_ins, start_depth + 1,
                                                   max_depth),
                               repeats);
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

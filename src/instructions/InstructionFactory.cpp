#include "instructions/InstructionFactory.hpp"
#include <limits>

std::vector<std::unique_ptr<IInstruction>>
InstructionFactory::create_instructions(const std::string &process_name,
                                        int num_instructions, int max_ins,
                                        int min_ins, int start_depth,
                                        int max_depth) {
  if (start_depth >= max_depth) {
    return {};
  }

  std::vector<std::unique_ptr<IInstruction>> instructions;
  int curr_instructions_count = 0;

  while (curr_instructions_count < num_instructions) {
    int instruction_type =
        rand() %
        (static_cast<int>(InstructionFactory::InstructionType::FOR) + 1);

    switch (
        static_cast<InstructionFactory::InstructionType>(instruction_type)) {
      case InstructionFactory::InstructionType::PRINT:
        instructions.push_back(InstructionFactory::create_print(
            "Hello from process " + process_name));

        ++curr_instructions_count;
        break;
      case InstructionFactory::InstructionType::DECLARE: {
        std::string var_name =
            "var" + std::to_string(rand() % num_instructions);
        uint16_t val = rand() % (std::numeric_limits<uint16_t>::max() + 1);

        instructions.push_back(
            InstructionFactory::create_declare(var_name, val));

        ++curr_instructions_count;
        break;
      }
      case InstructionFactory::InstructionType::ADD:
        instructions.push_back(InstructionFactory::create_arithmetic(
            "add_var", InstructionFactory::random_operand(),
            InstructionFactory::random_operand(), Arithmetic::Operator::ADD));

        ++curr_instructions_count;
        break;
      case InstructionFactory::InstructionType::SUBTRACT:
        instructions.push_back(InstructionFactory::create_arithmetic(
            "sub_var", InstructionFactory::random_operand(),
            InstructionFactory::random_operand(),
            Arithmetic::Operator::SUBTRACT));

        ++curr_instructions_count;
        break;
      case InstructionFactory::InstructionType::SLEEP: {
        uint8_t ticks = 1 + rand() % 5; // sleep between 1 and 5 ticks

        instructions.push_back(InstructionFactory::create_sleep(ticks));

        ++curr_instructions_count;
        break;
      }
      case InstructionFactory::InstructionType::FOR: {
        // can't have loop without 2 or more instructions
        if (curr_instructions_count >= num_instructions - 1)
          continue;

        ++curr_instructions_count; // loop itself counts as an instruction

        int remaining = num_instructions - curr_instructions_count;

        // 1 to 5 instructions
        int loop_instructions_count = std::min(remaining, 1 + rand() % 5);

        int remaining_repeats = remaining / loop_instructions_count;

        // 1 to 5 repeats
        int repeats = std::min(remaining_repeats, 1 + rand() % 5);

        instructions.push_back(InstructionFactory::create_for(
            process_name, loop_instructions_count, min_ins, max_ins, repeats,
            start_depth, max_depth));

        curr_instructions_count += loop_instructions_count * repeats;

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

  return std::make_unique<For>(
      create_instructions(process_name, num_instructions, max_ins, min_ins,
                          start_depth + 1, max_depth),
      repeats);
}

// helpers
Arithmetic::Operand InstructionFactory::random_operand() {
  int choice = rand() % 2;

  switch (choice) {
    case 0: // variable
      return Arithmetic::Operand("var" + std::to_string(rand() % 100));
    case 1: // constant
      return Arithmetic::Operand(uint16_t(rand() % 100));
    default:
      return Arithmetic::Operand(uint16_t(0));
  }
}

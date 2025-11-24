#include "instructions/InstructionFactory.hpp"
#include <iostream>
#include <limits>
#include <sstream>

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
      std::string var_name = "var" + std::to_string(rand() % num_instructions);
      uint16_t val = rand() % (std::numeric_limits<uint16_t>::max() + 1);

      instructions.push_back(InstructionFactory::create_declare(var_name, val));

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
      uint8_t ticks = 1 + rand() % 2; // sleep between 1 and 2 ticks

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

std::vector<std::unique_ptr<IInstruction>>
InstructionFactory::create_mo1_demo_instructions(
    const std::string &process_name, int num_instructions) {
  std::vector<std::unique_ptr<IInstruction>> instructions;
  int curr_instructions_count = 0;

  while (curr_instructions_count < num_instructions) {
    switch (curr_instructions_count % 2) {
    case 0: {
      instructions.push_back(std::make_unique<Print>("Value from: \"x\""));
      ++curr_instructions_count;
      break;
    }
    case 1: {
      uint16_t val = 1 + rand() % 10; // number between 1 and 10
      instructions.push_back(InstructionFactory::create_arithmetic(
          "x", Arithmetic::Operand("x"), Arithmetic::Operand(val),
          Arithmetic::Operator::ADD));
      ++curr_instructions_count;
      break;
    }
    }
  }

  return instructions;
}

std::vector<std::unique_ptr<IInstruction>>
InstructionFactory::create_instructions_from_string(const std::string &code) {
  // Mapper
  const std::unordered_map<std::string, InstructionType> command_mapper = {
      {"DECLARE", InstructionType::DECLARE},
      {"ADD", InstructionType::ADD},
      {"SUBTRACT", InstructionType::SUBTRACT},
      {"PRINT", InstructionType::PRINT},
      {"WRITE", InstructionType::WRITE},
      {"READ", InstructionType::READ}};

  std::vector<std::unique_ptr<IInstruction>> instructions;
  std::stringstream ss(code);
  std::string instruction_segment;

  // Split by semicolon
  while (std::getline(ss, instruction_segment, ';')) {
    instruction_segment = trim(instruction_segment);
    if (instruction_segment.empty())
      continue;

    // Use stringstream to extract command keyword
    std::stringstream line_ss(instruction_segment);
    std::string command_str;
    line_ss >> command_str;

    // Map to enum
    auto it = command_mapper.find(command_str);
    InstructionType command_type =
        (it != command_mapper.end()) ? it->second : InstructionType::UNKNOWN;

    switch (command_type) {
    case InstructionType::PRINT: {
      // Find content inside parenthesis: PRINT("Result: " + varC)
      size_t open_paren = instruction_segment.find('(');
      size_t close_paren = instruction_segment.find_last_of(')');

      if (open_paren != std::string::npos && close_paren != std::string::npos &&
          close_paren > open_paren) {
        std::string content = instruction_segment.substr(
            open_paren + 1, close_paren - open_paren - 1);
        instructions.push_back(create_print(content));
      }
      break;
    }
    case InstructionType::DECLARE: {
      std::string var_name;
      uint16_t value = 0;
      if (line_ss >> var_name >> value) {
        instructions.push_back(create_declare(var_name, value));
      }
      break;
    }
    case InstructionType::ADD:
    case InstructionType::SUBTRACT: {
      std::string dest, op1_str, op2_str;
      if (line_ss >> dest >> op1_str >> op2_str) {
        Arithmetic::Operator op = (command_type == InstructionType::ADD)
                                      ? Arithmetic::Operator::ADD
                                      : Arithmetic::Operator::SUBTRACT;
        instructions.push_back(create_arithmetic(dest, parse_operand(op1_str),
                                                 parse_operand(op2_str), op));
      }
      break;
    }
    case InstructionType::WRITE:
    case InstructionType::READ: {
      // Intentionally skipped for now as per requirements
      break;
    }
    case InstructionType::UNKNOWN:
    default: {
      std::cerr << "Warning: Unknown instruction command '" << command_str
                << "' in: " << instruction_segment << std::endl;
      break;
    }
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

std::string InstructionFactory::trim(const std::string &str) {
  size_t first = str.find_first_not_of(" \t\n\r");
  if (std::string::npos == first)
    return "";
  size_t last = str.find_last_not_of(" \t\n\r");
  return str.substr(first, (last - first + 1));
}

uint32_t InstructionFactory::parse_addr_val(const std::string &str) {
  if (str.find("0x") == 0 || str.find("0X") == 0) {
    return std::stoul(str, nullptr, 16);
  }
  return std::stoul(str);
}

Arithmetic::Operand
InstructionFactory::parse_operand(const std::string &token) {
  if (token.empty())
    return uint16_t(0);
  // Check if the token is a number
  if (std::isdigit(token[0])) {
    try {
      return static_cast<uint16_t>(std::stoul(token));
    } catch (...) {
      return uint16_t(0);
    }
  }
  // Otherwise treat as variable name
  return token;
}

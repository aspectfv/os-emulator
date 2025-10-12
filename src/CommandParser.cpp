#include "CommandParser.hpp"
#include <stdexcept>

void CommandParser::registerCommand(const std::string &name, Command command) {
  commands[name] = command;
}

void CommandParser::executeCommand(const std::string &input) {
  auto space_pos = input.find(' ');

  std::string command_name =
      (space_pos == std::string::npos) ? input : input.substr(0, space_pos);

  std::vector<std::string> args;

  if (space_pos != std::string::npos) {
    std::string args_str = input.substr(space_pos + 1);
    size_t pos = 0;
    while ((pos = args_str.find(' ')) != std::string::npos) {
      args.push_back(args_str.substr(0, pos));
      args_str.erase(0, pos + 1);
    }

    if (!args_str.empty()) {
      args.push_back(args_str);
    }
  }

  auto it = commands.find(command_name);

  if (it != commands.end()) {
    it->second(std::move(args));
  } else {
    throw std::runtime_error("Command not found: " + command_name);
  }
}

#include "CommandParser.hpp"
#include <stdexcept>

void CommandParser::registerCommand(const std::string &name, Command command) {
  commands[name] = command;
}

void CommandParser::executeCommand(const std::string &input) {
  auto space_pos = input.find(' ');

  std::string commandName =
      (space_pos == std::string::npos) ? input : input.substr(0, space_pos);
  std::string args =
      (space_pos == std::string::npos) ? "" : input.substr(space_pos + 1);

  auto it = commands.find(commandName);

  if (it != commands.end()) {
    it->second(args);
  } else {
    throw std::runtime_error("Command not found: " + commandName);
  }
}

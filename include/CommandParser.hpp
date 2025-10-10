#pragma once
#include <functional>
#include <string>

class CommandParser {
public:
  using Command = std::function<void(const std::string &)>;

  void registerCommand(const std::string &name, Command command) {
    commands[name] = command;
  }

  void executeCommand(const std::string &input) {
    auto spacePos = input.find(' ');

    std::string commandName =
        (spacePos == std::string::npos) ? input : input.substr(0, spacePos);
    std::string args =
        (spacePos == std::string::npos) ? "" : input.substr(spacePos + 1);

    auto it = commands.find(commandName);

    if (it != commands.end()) {
      it->second(args);
    } else {
      throw std::runtime_error("Command not found: " + commandName);
    }
  }

private:
  std::unordered_map<std::string, Command> commands;
};

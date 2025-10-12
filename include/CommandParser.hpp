#pragma once
#include <functional>
#include <string>
#include <unordered_map>

class CommandParser {
public:
  using Command = std::function<void(const std::vector<std::string> &&args)>;
  void registerCommand(const std::string &name, Command command);
  void executeCommand(const std::string &input);

private:
  std::unordered_map<std::string, Command> commands;
};

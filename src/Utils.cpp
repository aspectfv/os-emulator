#include "Utils.hpp"
#include <chrono>

std::string Utils::current_timestamp() {
  auto now = std::chrono::system_clock::now();
  std::time_t now_time = std::chrono::system_clock::to_time_t(now);
  std::tm *ltm = std::localtime(&now_time);

  char buffer[30];
  std::strftime(buffer, sizeof(buffer), "%m/%d/%Y %I:%M:%S%p", ltm);
  return std::string(buffer);
}

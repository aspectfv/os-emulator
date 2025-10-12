#pragma once
#include <string>

class Utils {
public:
  static std::string current_timestamp();

private:
  // static class - prevent instantiation
  Utils() = delete;
};

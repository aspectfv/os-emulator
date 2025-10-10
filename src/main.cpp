#include "Emulator.hpp"
#include <iostream>
#include <string>

int main() {
  Emulator emulator;
  std::string input;

  while (true) {
    std::cout << "\nroot:> ";
    std::cout.flush();

    if (!std::getline(std::cin, input))
      break;

    if (!emulator.process_input(input))
      break;
  }

  return 0;
}

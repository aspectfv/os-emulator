#include "instructions/Print.hpp"
#include <iostream>

Print::Print(const std::string &msg) : message(msg) {}

void Print::execute() { std::cout << message << std::endl; }

#include <iostream>

#include "Command.hpp"

using namespace eraxc::JIR;

void Command::print() const {
    std::cout << operationToString(op) << " " << operand1.to_string() << ' ' << operand2.to_string() << std::endl;
}
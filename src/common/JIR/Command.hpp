#pragma once

#include "Operand.hpp"
#include "Operation.hpp"

namespace eraxc::JIR {

    class Command {
    public:
        Operation op = Operation::ERR;
        Operand operand1;
        Operand operand2;

        void print() const;
    };
}

#pragma once

#include "Operand.h"
#include "Operation.h"

namespace eraxc::JIR {
    struct Node {
        Operation op = Operation::ERR;
        Operand operand1;
        Operand operand2;
    };
}

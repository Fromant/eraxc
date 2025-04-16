#ifndef JIR_NODE
#define JIR_NODE

#include "operand.h"
#include "Operation.h"

namespace eraxc::JIR {
    struct Node {
        Operation op = Operation::ERR;
        Operand operand1;
        Operand operand2;
    };
}

#endif
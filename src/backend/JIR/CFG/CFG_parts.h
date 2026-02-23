#pragma once

#include <vector>

#include "../../Scope.h"
#include "backend/JIR/Operation.h"

namespace eraxc::JIR {

    struct Operand {
        u64 type;
        u64 value;
        bool is_instant;
        bool is_rvalue;
    };

    struct JIRop {
        Operation op = Operation::ERR;
        Operand operand1;
        Operand operand2;
    };

    struct CFG_Node {
        std::vector<JIRop> body;
        Scope scope;
    };

    enum CFGEdgeType {
        SQUASH,
        EXTEND
    };

    struct CFGEdge {
        size_t to_id;
        CFGEdgeType type;
        Operation jump_op = Operation::JUMP;
    };

    struct CFG_Func {
        u64 return_type;
        u64 node_id;
        size_t max_stack_size;
        std::vector<Operand> params;
    };
}

#pragma once

#include <vector>

#include "../../Scope.h"

namespace eraxc::JIR {

    struct CFG_Node {
        std::vector<JIRop> body;
    };

    enum CFGEdgeType {
        SQUASH, EXTEND
    };

    struct CFG_Edge {
        size_t from_id;
        size_t id_to;
        size_t id_control_instruction;
        CFGEdgeType type;
    };

    struct CFG_Func {
        u64 return_type;
        u64 node_id;
        size_t max_stack_size;
        std::vector<Operand> params;
    };
}

#pragma once

#include <vector>

#include "../../Scope.h"

namespace eraxc::JIR {

    struct CFG_Node {
        std::vector<Node> body;
        // std::vector<Operand> allocations;
    };

    struct CFG_Edge {
        size_t from_id;
        size_t id_to;
        size_t id_control_instruction;
    };

    struct CFG_Func {
        u64 return_type;
        u64 node_id;
        std::vector<Operand> params;
    };
}

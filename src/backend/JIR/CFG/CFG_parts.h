#ifndef CFG_PARTS_H
#define CFG_PARTS_H

#include <vector>
#include "../../scope.h"

namespace eraxc::JIR {

    struct CFG_Node {
        std::vector<Node> body;
        std::vector<Operand> allocations;
        size_t scope_id;
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

#endif //CFG_PARTS_H

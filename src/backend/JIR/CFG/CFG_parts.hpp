#pragma once

#include <vector>

#include "../../Scope.hpp"
#include "backend/JIR/Operation.hpp"

namespace eraxc::JIR {

    struct Operand {
        u64 type;
        u64 value;
        bool is_instant;
        bool is_rvalue;

        Operand() {
            is_instant = false;
            is_rvalue = false;
            type = -1;
            value = -1;
        }

        Operand(u64 type, u64 value, bool is_instant, bool is_rvalue) :
            type(type), value(value), is_instant(is_instant), is_rvalue(is_rvalue) {}
    };

    struct JIRop {
        Operation op = Operation::ERR;
        Operand operand1;
        Operand operand2;
    };

    struct CFG_Node {
        std::vector<JIRop> body;
        // {type, id}
        std::vector<std::pair<u64, u64>> allocatedIds;
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

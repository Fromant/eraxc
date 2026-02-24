#pragma once
#include "backend/JIR/Operation.hpp"
#include "util/common.hpp"

namespace eraxc::JIR::Allocated {
    // stack or register allocated variable
    struct OperandAllocated {
        u64 type;
        u64 value;  //stack offset or register ID
        bool is_stack_allocated;
        bool is_instant;
    };

    struct JIRAOp {
        Operation op = Operation::ERR;
        OperandAllocated operand1 {};
        OperandAllocated operand2 {};
    };

    using Nodes = std::vector<JIRAOp>;

    struct CFGA_Node {
        Nodes body;
    };
}
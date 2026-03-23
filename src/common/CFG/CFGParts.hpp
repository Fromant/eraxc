#pragma once
#include <unordered_map>
#include <vector>

#include "../JIR/Operation.hpp"
#include "common/JIR/Command.hpp"
#include "util/common.hpp"


namespace eraxc::CFG {

    using CFGNode = std::vector<JIR::Command>;

    // class CFGNode {
    // public:
    //     std::vector<JIR::Command> nodes;
    //     CFGNode() = default;
    //     CFGNode(const std::vector<JIR::Command>& nodes) : nodes(nodes) {};
    //
    //     void print() const;
    // };
    // struct CFGNode {
    //
    //     // {type, id}
    //     std::vector<std::pair<u64, u64>> allocatedIds;
    // };

    enum CFGEdgeType {
        SQUASH,
        EXTEND
    };

    struct CFGEdge {
        size_t to_id;
        CFGEdgeType type;
        JIR::Operation jump_op = JIR::Operation::JUMP;
    };

    struct CFGFunc {
        JIR::Type return_type;
        u64 node_id;
        size_t max_stack_size;
        std::vector<JIR::Operand> params;
    };

    class CFG {
    public:
        size_t maxStackSize = -1;
        std::vector<CFGNode> nodes;
        // {from_id, edge}
        std::unordered_map<size_t, std::vector<CFGEdge>> edges;
    };
}
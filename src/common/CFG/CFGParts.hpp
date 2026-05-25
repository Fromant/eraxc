#pragma once
#include <unordered_map>
#include <vector>

#include "../JIR/Operation.hpp"
#include "common/JIR/Command.hpp"
#include "common/JIR/Declaration.hpp"
#include "util/common.hpp"


namespace eraxc::CFG {

    class CFGNode {
    public:
        std::vector<JIR::Command> commands;
        CFGNode() = default;
        CFGNode(const std::vector<JIR::Command>& commands) : commands(commands) {};

        std::vector<JIR::Declaration> declarations;

        void operator+=(const CFGNode& node);
    };

    struct CFGEdge {
        enum CFGEdgeType {
            SQUASH,
            EXTEND
        };

        size_t to_id = -1;
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
        using NodeType = CFGNode;
        using CommandType = JIR::Command;
        using EdgeType = CFGEdge;

        size_t maxStackSize = -1;
        std::vector<CFGNode> nodes;
        // {from_id, edge}
        std::unordered_map<size_t, std::vector<CFGEdge>> edges;
    };
}
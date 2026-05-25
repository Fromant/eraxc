#pragma once
#include <unordered_map>

#include "common/CFG/CFGParts.hpp"
#include "common/JIR/allocated/Command.hpp"

namespace eraxc::CFG::allocated {

    struct CFGNode {
        std::vector<JIR::allocated::Command> commands;
    };

    class CFG {
    public:
        using NodeType = CFGNode;
        using CommandType = JIR::allocated::Command;
        using EdgeType = CFGEdge;

        CFG() = default;

        size_t maxStackSize = 0;

        std::vector<CFGNode> nodes;
        // {from_id, edge}
        std::unordered_map<size_t, std::vector<CFGEdge>> edges;
    };
}

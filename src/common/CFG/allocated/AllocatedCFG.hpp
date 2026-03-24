#pragma once
#include "common/JIR/allocated/Command.hpp"
namespace eraxc::CFG::allocated {

    struct CFGNode {
        std::vector<JIR::allocated::Command> commands;
    };

    class CFG {
    public:
        CFG() = default;

        std::vector<CFGNode> nodes;
        // {from_id, edge}
        std::unordered_map<size_t, std::vector<CFGEdge>> edges;
    };
}

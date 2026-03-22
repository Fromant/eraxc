#pragma once
#include <queue>
#include <set>
#include <vector>

#include "CFGParts.hpp"

namespace eraxc::CFG {
    class CFGIterator {
        const CFG& cfg;
        std::queue<size_t> q;
        std::set<size_t> visited_nodes;

    public:
        CFGIterator(const CFG& cfg, size_t node_id);

        const CFGNode& operator*() const;

        size_t nodeId() const;

        std::vector<CFGEdge> getEdges() const;

        CFGIterator& operator++();

        explicit operator bool() const;
    };
}

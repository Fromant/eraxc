#pragma once
#include <queue>

#include "../CFGAllocated.h"

namespace eraxc::JIR {

    class CFGIterator {
        const Allocated::CFGAllocated& cfg;
        std::queue<size_t> q;
        std::set<size_t> visited_nodes;

    public:
        CFGIterator(const Allocated::CFGAllocated& cfg, size_t node_id);

        const Allocated::CFGA_Node& operator*() const;

        size_t nodeId() const;

        std::vector<CFGEdge> getEdges() const;

        CFGIterator& operator++();

        explicit operator bool() const;
    };
}
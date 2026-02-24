#pragma once
#include <queue>

#include "CFG.h"

namespace eraxc::JIR {

    class CFGIterator {
        const Allocated::CFGAllocated& cfg;
        std::queue<size_t> q;
        std::set<size_t> visited_nodes;

    public:
        CFGIterator(const Allocated::CFGAllocated& cfg, size_t node_id) : cfg(cfg) {
            q.push(node_id);
            visited_nodes.emplace(node_id);
        }

        const Allocated::CFGA_Node& operator*() const {
            return cfg.getCfgNode(q.front());
        }

        size_t nodeId() const {
            return q.front();
        }

        std::vector<CFGEdge> getEdges() const {
            const auto& edgesMap = cfg.getEdges();

            const auto edges = edgesMap.find(q.front());
            if (edges == edgesMap.end()) {
                return {};
            }
            return edges->second;
        }

        CFGIterator& operator++() {
            size_t current_id = q.front();
            q.pop();

            const auto& edgesMap = cfg.getEdges();

            const auto edges = edgesMap.find(current_id);
            if (edges == edgesMap.end()) {
                return *this;
            }
            for (const auto& edge : edges->second) {
                auto [child_it, child_inserted] = visited_nodes.emplace(edge.to_id);
                if (child_inserted) {
                    q.push(edge.to_id);
                }
            }
            return *this;
        }

        explicit operator bool() const {
            return !q.empty();
        }
    };
}
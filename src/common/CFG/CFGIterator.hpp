#pragma once
#include <queue>
#include <set>
#include <unordered_map>
#include <vector>

namespace eraxc::CFG {

    template<typename T>
    concept CFGLike = requires(const T& cfg, size_t id) {
        // Must have these nested types
        typename T::NodeType;
        typename T::EdgeType;

        // Must have these members with correct types
        { cfg.nodes } -> std::same_as<const std::vector<typename T::NodeType>&>;
        { cfg.edges } -> std::same_as<const std::unordered_map<size_t, std::vector<typename T::EdgeType>>&>;
    };

    template<CFGLike CFGType>
    class CFGIterator {
        const CFGType& cfg;
        std::queue<size_t> q;
        std::set<size_t> visited_nodes;

    public:
        using NodeType = CFGType::NodeType;
        using EdgeType = CFGType::EdgeType;

        CFGIterator(const CFGType& cfg, size_t node_id);

        const NodeType& operator*() const;

        size_t nodeId() const;

        std::vector<EdgeType> getEdges() const;

        CFGIterator& operator++();

        explicit operator bool() const;
    };

    // Implementation
    template<CFGLike CFGType>
    CFGIterator<CFGType>::CFGIterator(const CFGType& cfg, size_t node_id) : cfg(cfg) {
        q.push(node_id);
        visited_nodes.emplace(node_id);
    }

    template<CFGLike CFGType>
    const CFGIterator<CFGType>::NodeType& CFGIterator<CFGType>::operator*() const {
        return cfg.nodes[q.front()];
    }

    template<CFGLike CFGType>
    size_t CFGIterator<CFGType>::nodeId() const {
        return q.front();
    }

    template<CFGLike CFGType>
    std::vector<typename CFGIterator<CFGType>::EdgeType> CFGIterator<CFGType>::getEdges() const {
        auto& edgesMap = cfg.edges;

        const auto& edges = edgesMap.find(q.front());
        if (edges == edgesMap.end()) {
            return {};
        }
        return edges->second;
    }

    template<CFGLike CFGType>
    CFGIterator<CFGType>& CFGIterator<CFGType>::operator++() {
        size_t current_id = q.front();
        q.pop();

        const auto& edgesMap = cfg.edges;

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

    template<CFGLike CFGType>
    CFGIterator<CFGType>::operator bool() const {
        return !q.empty() && !cfg.nodes.empty();
    }
}

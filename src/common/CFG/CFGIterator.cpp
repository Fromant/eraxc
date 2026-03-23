#include "CFGIterator.hpp"

using namespace eraxc::CFG;

CFGIterator::CFGIterator(const CFG& cfg, size_t node_id) : cfg(cfg) {
    q.push(node_id);
    visited_nodes.emplace(node_id);
}

const CFGNode& CFGIterator::operator*() const {
    return cfg.nodes[q.front()];
}

size_t CFGIterator::nodeId() const {
    return q.front();
}

std::vector<CFGEdge> CFGIterator::getEdges() const {
    auto& edgesMap = cfg.edges;

    const auto& edges = edgesMap.find(q.front());
    if (edges == edgesMap.end()) {
        return {};
    }
    return edges->second;
}

CFGIterator& CFGIterator::operator++() {
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

CFGIterator::operator bool() const {
    return !q.empty() && !cfg.nodes.empty();
}
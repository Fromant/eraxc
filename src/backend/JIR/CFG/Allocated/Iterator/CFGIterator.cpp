#include "CFGIterator.h"

eraxc::JIR::CFGIterator::CFGIterator(const Allocated::CFGAllocated& cfg, size_t node_id) : cfg(cfg) {
    q.push(node_id);
    visited_nodes.emplace(node_id);
}

const eraxc::JIR::Allocated::CFGA_Node& eraxc::JIR::CFGIterator::operator*() const {
    return cfg.getCfgNode(q.front());
}

size_t eraxc::JIR::CFGIterator::nodeId() const {
    return q.front();
}

std::vector<eraxc::JIR::CFGEdge> eraxc::JIR::CFGIterator::getEdges() const {
    const auto& edgesMap = cfg.getEdges();

    const auto edges = edgesMap.find(q.front());
    if (edges == edgesMap.end()) {
        return {};
    }
    return edges->second;
}

eraxc::JIR::CFGIterator& eraxc::JIR::CFGIterator::operator++() {
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

eraxc::JIR::CFGIterator::operator bool() const {
    return !q.empty();
}
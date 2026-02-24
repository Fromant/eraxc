#pragma once
#include <map>
#include <unordered_map>
#include <vector>

#include "CFGTypes.hpp"
#include "backend/JIR/CFG/Allocated/AllocationManager.hpp"
#include "backend/JIR/CFG/CFG.hpp"
#include "backend/JIR/CFG/CFG_parts.hpp"

namespace eraxc::JIR::Allocated {

    class CFGAllocated {

        EdgesMap edges;
        std::vector<CFGA_Node> nodes;
        std::map<u64, CFG_Func> funcs;
        ScopeManager scope;

    public:
        explicit CFGAllocated(const CFG& cfg) : edges(cfg.get_edges()), funcs(cfg.get_funcs()), scope(cfg.getScopeManager()) {
            nodes.resize(cfg.get_nodes().size());

            for (const auto& func : funcs | std::views::values) {
                AllocationManager a(nodes, cfg.get_nodes(), cfg.get_edges());
                auto r = a.create(func);
                if (!r) {
                    throw std::runtime_error("Could not create node allocation manager: " + r.error);
                }
            }
        }

        const ScopeManager& getScopeManager() const {
            return scope;
        }

        const auto& getFuncs() const {
            return funcs;
        }

        const auto& getCfgNode(size_t node_id) const {
            return nodes[node_id];
        }

        const auto& getEdges() const {
            return edges;
        }
    };
}

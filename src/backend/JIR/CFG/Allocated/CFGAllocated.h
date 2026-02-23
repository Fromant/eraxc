#pragma once
#include <map>
#include <unordered_map>
#include <vector>

#include "CFGTypes.h"
#include "backend/JIR/CFG/Allocated/AllocationManager.h"
#include "backend/JIR/CFG/CFG.h"
#include "backend/JIR/CFG/CFG_parts.h"

namespace eraxc::JIR::Allocated {

    class CFGAllocated {

        EdgesMap edges;
        std::vector<CFGA_Node> nodes;
        std::map<u64, CFG_Func> funcs;

    public:
        explicit CFGAllocated(const CFG& cfg) : edges(cfg.get_edges()), funcs(cfg.get_funcs()) {
            nodes.resize(cfg.get_nodes().size());

            for (const auto& func : funcs | std::views::values) {
                AllocationManager a(nodes, cfg.get_nodes(), cfg.get_edges());
                auto r = a.create(func);
                if (!r) {
                    throw std::runtime_error("Could not create node allocation manager: " + r.error);
                }
            }
        }
    };
}

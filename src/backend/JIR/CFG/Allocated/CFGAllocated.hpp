// #pragma once
// #include <map>
// #include <unordered_map>
// #include <vector>
//
// #include "CFGTypes.hpp"
// #include "backend/JIR/CFG/Allocated/AllocationManager.hpp"
// #include "backend/JIR/CFG/CFG.hpp"
// #include "backend/JIR/CFG/CFG_parts.hpp"
//
// namespace eraxc::JIR::Allocated {
//
//     class CFGAllocated {
//
//         EdgesMap edges;
//         std::vector<CFGA_Node> nodes;
//         std::map<u64, CFG_Func> funcs;
//         ScopeManager scope;
//         std::vector<Scope::Declaration> globals;
//
//     public:
//         explicit CFGAllocated(const CFG& cfg) :
//             edges(cfg.get_edges()), funcs(cfg.get_funcs()), scope(cfg.getScopeManager()), globals(cfg.getGlobals()) {
//             nodes.resize(cfg.get_nodes().size());
//
//             for (const auto& func : funcs | std::views::values) {
//                 AllocationManager a(nodes, cfg.get_nodes(), cfg.get_edges(), globals);
//                 auto r = a.create(func);
//                 if (!r) {
//                     throw std::runtime_error("Could not create node allocation manager: " + r.error);
//                 }
//             }
//
//             AllocationManager globalsAlloc(nodes, cfg.get_nodes(), cfg.get_edges(), globals);
//             const auto& void_type = cfg.getScopeManager().findTypeRecursive("void");
//             if (!void_type) {
//                 throw std::runtime_error("Could not find type void");
//             }
//             auto err = globalsAlloc.create({void_type.value(), 0, cfg.getScopeManager().top().getAllocatedSize(), {}});
//             if (!err) {
//                 throw std::runtime_error("CFG allocation: " + err.error);
//             }
//         }
//
//         const ScopeManager& getScopeManager() const {
//             return scope;
//         }
//
//         const auto& getFuncs() const {
//             return funcs;
//         }
//
//         const auto& getCfgNode(size_t node_id) const {
//             return nodes[node_id];
//         }
//
//         const auto& getEdges() const {
//             return edges;
//         }
//
//         const auto& getGlobals() const {
//             return globals;
//         }
//     };
// }

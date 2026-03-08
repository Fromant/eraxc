#pragma once

#include "../../../codegen/x64/asm_x64_mem.hpp"
#include "CFGTypes.hpp"
#include "backend/JIR/CFG/CFG.hpp"
#include "backend/JIR/CFG/CFG_parts.hpp"

using namespace eraxc::x64;

namespace eraxc::JIR::Allocated {

    // manages allocations of variables in amd64 context
    // both stack and registers
    // takes CFG as an input and created CFG with allocations
    class AllocationManager {

        std::set<x86_reg> free_regs {x86_reg::RAX, x86_reg::RBX, x86_reg::RCX, x86_reg::RDX, x86_reg::RDI,
                                     x86_reg::RSI, x86_reg::R8,  x86_reg::R9,  x86_reg::R10, x86_reg::R11,
                                     x86_reg::R12, x86_reg::R13, x86_reg::R14, x86_reg::R15};

        //For mapping used vars to regs
        std::unordered_map<u64, x86_reg> used_regs {};

        u64 used_stack_space = 0;
        u64 total_stack_space = 0;

        //For mapping used vars to stack
        std::unordered_map<u64, u64> stack_offsets {};

        std::set<u64> globals {};

        std::set<size_t> allocated_nodes {};

        std::vector<CFGA_Node>& nodes;
        const std::vector<CFG_Node>& old_nodes;
        const EdgesMap& edgesMap;

        error::errable<OperandAllocated> operandToAllocated(const Operand& operand);

        error::errable<void> deallocVar(u64 type, u64 id);

        error::errable<void> allocVar(u64 type, u64 id);

        error::errable<void> CFGNodeToCFGANode(const CFG_Node& old_node, CFGA_Node& node);

        // Allocates all CFG subgraph starting with node_id
        error::errable<void> allocatedCfgNode(size_t node_id);

        void clear();

        size_t args_in_registers_count = 0;

    public:
        // allocate cfg func
        AllocationManager(std::vector<CFGA_Node>& nodes, const std::vector<CFG_Node>& old_nodes, const EdgesMap& edges,
                          const std::vector<Scope::Declaration>& globals) :
            nodes(nodes), old_nodes(old_nodes), edgesMap(edges) {
            for (const auto& decl : globals) {
                this->globals.emplace(decl.getId());
            }
        }


        error::errable<void> create(const CFG_Func& f);
    };
}

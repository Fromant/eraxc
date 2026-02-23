#pragma once

#include <queue>

#include "backend/JIR/CFG/CFG.h"
#include "backend/JIR/CFG/CFG_parts.h"

namespace eraxc::JIR::Allocated {

    // manages allocations of variables in amd64 context
    // both stack and registers
    // takes CFG as an input and created CFG with allocations
    class AllocationManager {
        enum class x86_reg {
            RAX,
            RBX,
            RCX,
            RDX,

            RBP,
            RSP,
            RSI,
            RDI,

            R8,
            R9,
            R10,
            R11,
            R12,
            R13,
            R14,
            R15
        };

        static std::string reg_name(x86_reg reg, u64 type_size) {
            std::string r;
            if (type_size == 8) {
                r = "r";
            } else if (type_size == 4) {
                r = "e";
            } else
                return {"ILLREG SIZE"};
            switch (reg) {
                case x86_reg::RAX: r += "ax"; break;
                case x86_reg::RBX: r += "bx"; break;
                case x86_reg::RCX: r += "cx"; break;
                case x86_reg::RDX: r += "dx"; break;
                case x86_reg::RBP: r += "bp"; break;
                case x86_reg::RSP: r += "sp"; break;
                case x86_reg::RSI: r += "si"; break;
                case x86_reg::RDI: r += "di"; break;
                case x86_reg::R8: r += "8"; break;
                case x86_reg::R9: r += "9"; break;
                case x86_reg::R10: r += "10"; break;
                case x86_reg::R11: r += "11"; break;
                case x86_reg::R12: r += "12"; break;
                case x86_reg::R13: r += "13"; break;
                case x86_reg::R14: r += "14"; break;
                case x86_reg::R15: r += "15"; break;
                default: return "ILLREG";
            }
            return r;
        }

        static constexpr x86_reg pass_ABI[] = {x86_reg::RCX, x86_reg::RDX, x86_reg::R8, x86_reg::R9};

        std::set<x86_reg> free_regs {x86_reg::RAX, x86_reg::RBX, x86_reg::RCX, x86_reg::RDX, x86_reg::RDI,
                                     x86_reg::RSI, x86_reg::R8,  x86_reg::R9,  x86_reg::R10, x86_reg::R11,
                                     x86_reg::R12, x86_reg::R13, x86_reg::R14, x86_reg::R15};

        //For mapping used vars to regs
        std::unordered_map<u64, x86_reg> used_regs {};

        u64 used_stack_space = 0;

        //For mapping used vars to stack
        std::unordered_map<u64, u64> stack_offsets {};

        std::set<u64> globals {};

        std::set<size_t> allocated_nodes {};

        std::vector<CFGA_Node>& nodes;
        const std::vector<CFG_Node>& old_nodes;
        const EdgesMap& edgesMap;

        error::errable<OperandAllocated> operandToAllocated(const Operand& operand) {

            if (operand.value == -1 && operand.type == -1 && operand.is_instant == false &&
                operand.is_rvalue == false) {
                return {"", {}};
            }

            if (operand.is_instant) {
                const OperandAllocated op {operand.type, operand.value, false, true};
                return error::errable {op};
            }

            if (const auto regs_it = used_regs.find(operand.value); regs_it != used_regs.end()) {
                // value = register id
                return error::errable {OperandAllocated {operand.type, (u64)regs_it->second, false, false}};
            }

            if (const auto stack_it = stack_offsets.find(operand.value); stack_it != stack_offsets.end()) {
                // value = offset
                return error::errable {OperandAllocated {operand.type, stack_it->second, true, false}};
            }

            return {"Operand $" + std::to_string(operand.value) + " is not allocated", {}};
        }

        error::errable<void> deallocVar(u64 type, u64 id) {
            const u64 size = x86::size(type);

            if (used_regs.erase(id)) {
                return "";
            }

            const auto stack_it = stack_offsets.find(id);
            if (stack_it == stack_offsets.end()) {
                return "Error while deallocating $" + std::to_string(id) + ": Variable is not allocated";
            }

            if (stack_it->second + size != used_stack_space) {
                return "Error while deallocating $" + std::to_string(id) + ": Variable is not on top of stack";
            }

            stack_offsets.erase(stack_it);

            return "Error while deallocating $" + std::to_string(id) + ": Variable is not allocated";
        }

        error::errable<void> allocVar(u64 type, u64 id) {
            const u64 size = x86::size(type);
            if (stack_offsets.contains(id)) {
                return {"Variable $" + std::to_string(id) + " is already allocated"};
            }
            stack_offsets.emplace(id, used_stack_space);
            used_stack_space += size;
            return {""};
        }

        error::errable<void> CFGNodeToCFGANode(const CFG_Node& old_node, CFGA_Node& node) {
            node.body.reserve(old_node.body.size());

            for (const auto& op : old_node.body) {
                // skip alloc and dealloc
                if (op.op == Operation::ALLOC || op.op == Operation::DEALLOC) {
                    continue;
                }

                if (op.op == Operation::CALL) {
                    // second operand is a result of call
                    auto op2 = operandToAllocated(op.operand2);
                    if (!op2) {
                        return {op2.error};
                    }
                    used_regs.emplace(op2.value.value, x86_reg::RAX);
                    return {""};
                }

                if (op.op == Operation::RET) {
                    node.body.emplace_back(Operation::STACKDEALLOC,
                                           OperandAllocated {0, used_stack_space, false, false}, OperandAllocated {});
                    node.body.emplace_back(Operation::RET, OperandAllocated {}, OperandAllocated {});
                    continue;
                }


                const auto& operand1 = operandToAllocated(op.operand1);
                if (!operand1) {
                    return operand1.error;
                }
                const auto& operand2 = operandToAllocated(op.operand2);
                if (!operand2) {
                    return operand2.error;
                }
                node.body.emplace_back(op.op, operand1.value, operand2.value);
            }
            return {""};
        }

        // Allocates all CFG subgraph starting with node_id
        error::errable<void> allocatedCfgNode(const size_t node_id) {
            // insert and check
            auto [it, inserted] = allocated_nodes.emplace(node_id);
            if (!inserted) {
                return {""};
            }

            // BFS queue
            std::queue<size_t> q;
            q.push(node_id);

            while (!q.empty()) {
                size_t current_id = q.front();
                q.pop();

                const CFG_Node& old_node = old_nodes[current_id];
                CFGA_Node& new_node = nodes[current_id];

                for (const auto& [type, id] : old_node.allocatedIds) {
                    allocVar(type, id);
                }

                if (const auto& err = CFGNodeToCFGANode(old_node, new_node); !err) {
                    return err;
                }

                const auto edges = edgesMap.find(current_id);
                if (edges == edgesMap.end()) {
                    continue;
                }
                // Iterate through edges
                for (const auto& edge : edges->second) {
                    if (edge.type == SQUASH) {
                        // dealloc stack
                        for (const auto& [type, id] : old_node.allocatedIds | std::views::reverse) {
                            deallocVar(type, id);
                        }
                    }

                    // Add to queue if not visited
                    auto [child_it, child_inserted] = allocated_nodes.emplace(edge.to_id);
                    if (child_inserted) {
                        q.push(edge.to_id);
                    }
                }
            }
            return {""};
        }

        void clear() {
            stack_offsets.clear();
            used_regs.clear();
            used_stack_space = 0;
            allocated_nodes.clear();
        }

    public:
        // allocate cfg func
        AllocationManager(std::vector<CFGA_Node>& nodes, const std::vector<CFG_Node>& old_nodes,
                          const EdgesMap& edges) :
            nodes(nodes), old_nodes(old_nodes), edgesMap(edges) {}


        error::errable<void> create(const CFG_Func& f) {
            // allocate parameters
            size_t args_in_registers_count = 0;
            for (const auto& param : f.params) {
                used_regs.emplace(param.value, pass_ABI[args_in_registers_count++]);
            }

            nodes[f.node_id].body.emplace_back(Operation::STACKALLOC, OperandAllocated {0, f.max_stack_size, false},
                                               OperandAllocated {});
            const auto r = allocatedCfgNode(f.node_id);
            return r;
        }
    };
}

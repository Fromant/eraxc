// #include "AllocationManager.hpp"
//
// #include <queue>
//
// #include "CFGTypes.hpp"
//
// error::errable<eraxc::JIR::Allocated::OperandAllocated> eraxc::JIR::Allocated::AllocationManager::operandToAllocated(
//     const Operand& operand) {
//
//     if (operand.value == -1 && operand.type == -1 && operand.is_instant == false && operand.is_rvalue == false) {
//         return {"", {}};
//     }
//
//     if (operand.is_instant) {
//         const OperandAllocated op {operand.type, operand.value, OperandAllocated::INSTANT};
//         return error::errable {op};
//     }
//
//     if (const auto globals_it = globals.find(operand.value); globals_it != globals.end()) {
//         // value = global id
//         return error::errable {OperandAllocated {operand.type, operand.value, OperandAllocated::GLOBAL}};
//     }
//
//     if (const auto regs_it = used_regs.find(operand.value); regs_it != used_regs.end()) {
//         // value = register id
//         return error::errable {OperandAllocated {operand.type, (u64)regs_it->second, OperandAllocated::REGISTER}};
//     }
//
//     if (const auto stack_it = stack_offsets.find(operand.value); stack_it != stack_offsets.end()) {
//         // value = offset
//         return error::errable {OperandAllocated {operand.type, stack_it->second, OperandAllocated::STACK}};
//     }
//
//     return {"Operand $" + std::to_string(operand.value) + " is not allocated", {}};
// }
// error::errable<void> eraxc::JIR::Allocated::AllocationManager::deallocVar(u64 type, u64 id) {
//     const u64 size = x64::size(type);
//
//     if (used_regs.erase(id)) {
//         return "";
//     }
//
//     const auto stack_it = stack_offsets.find(id);
//     if (stack_it == stack_offsets.end()) {
//         return "Error while deallocating $" + std::to_string(id) + ": Variable is not allocated";
//     }
//
//     if (stack_it->second + size != used_stack_space) {
//         return "Error while deallocating $" + std::to_string(id) + ": Variable is not on top of stack";
//     }
//
//     stack_offsets.erase(stack_it);
//
//     return "Error while deallocating $" + std::to_string(id) + ": Variable is not allocated";
// }
// error::errable<void> eraxc::JIR::Allocated::AllocationManager::allocVar(u64 type, u64 id) {
//     const u64 size = x64::size(type);
//     if (stack_offsets.contains(id)) {
//         return {"Variable $" + std::to_string(id) + " is already allocated"};
//     }
//     stack_offsets.emplace(id, total_stack_space);
//     total_stack_space -= size;
//     if (total_stack_space < 0) {
//         return {"Internal error: exceeding stack size"};
//     }
//     return {""};
// }
// error::errable<void> eraxc::JIR::Allocated::AllocationManager::CFGNodeToCFGANode(const CFG_Node& old_node,
//                                                                                  CFGA_Node& node) {
//     node.body.reserve(old_node.body.size());
//
//     for (const auto& op : old_node.body) {
//         // skip alloc and dealloc
//         if (op.op == Operation::ALLOC || op.op == Operation::DEALLOC || op.op == Operation::STACKDEALLOC ||
//             op.op == Operation::STACKALLOC) {
//             continue;
//         }
//
//         if (op.op == Operation::PASS_RET) {
//             const auto returnable = operandToAllocated(op.operand1);
//             if (!returnable) {
//                 return returnable.error;
//             }
//             node.body.emplace_back(
//                 Operation::MOVE,
//                 OperandAllocated {returnable.value.type, (u64)x86_reg::RAX, OperandAllocated::REGISTER},
//                 returnable.value);
//             continue;
//         }
//
//         if (op.op == Operation::PASS) {
//             const auto passed = operandToAllocated(op.operand1);
//             if (!passed) {
//                 return passed.error;
//             }
//             node.body.emplace_back(Operation::MOVE,
//                                    OperandAllocated {passed.value.type, (u64)pass_ABI[args_in_registers_count++],
//                                                      OperandAllocated::REGISTER},
//                                    passed.value);
//             continue;
//         }
//
//         if (op.op == Operation::CALL) {
//             args_in_registers_count = 0;
//
//             node.body.emplace_back(Operation::CALL,
//                                    OperandAllocated {(u64)-1, op.operand1.value, OperandAllocated::INSTANT},
//                                    OperandAllocated {});
//
//             if (op.operand2.type != syntax::VOID) {
//                 // second operand is a result of call
//                 auto op2 = operandToAllocated(op.operand2);
//                 if (!op2) {
//                     return {op2.error};
//                 }
//                 node.body.emplace_back(
//                     Operation::MOVE, op2.value,
//                     OperandAllocated {op2.value.type, (u64)x86_reg::RAX, OperandAllocated::REGISTER});
//             }
//             continue;
//         }
//
//         if (op.op == Operation::RET) {
//             node.body.emplace_back(Operation::STACKDEALLOC,
//                                    OperandAllocated {0, used_stack_space, OperandAllocated::INSTANT},
//                                    OperandAllocated {});
//             node.body.emplace_back(Operation::RET, OperandAllocated {}, OperandAllocated {});
//             continue;
//         }
//
//
//         const auto& operand1 = operandToAllocated(op.operand1);
//         if (!operand1) {
//             return operand1.error;
//         }
//         const auto& operand2 = operandToAllocated(op.operand2);
//         if (!operand2) {
//             return operand2.error;
//         }
//         node.body.emplace_back(op.op, operand1.value, operand2.value);
//     }
//     return {""};
// }
// error::errable<void> eraxc::JIR::Allocated::AllocationManager::allocatedCfgNode(const size_t node_id) {
//     // insert and check
//     auto [it, inserted] = allocated_nodes.emplace(node_id);
//     if (!inserted) {
//         return {""};
//     }
//
//     // BFS queue
//     std::queue<size_t> q;
//     q.push(node_id);
//
//     while (!q.empty()) {
//         size_t current_id = q.front();
//         q.pop();
//
//         const CFG_Node& old_node = old_nodes[current_id];
//         CFGA_Node& new_node = nodes[current_id];
//
//         for (const auto& [type, id] : old_node.allocatedIds) {
//             allocVar(type, id);
//         }
//
//         if (const auto& err = CFGNodeToCFGANode(old_node, new_node); !err) {
//             return err;
//         }
//
//         const auto edges = edgesMap.find(current_id);
//         if (edges == edgesMap.end()) {
//             continue;
//         }
//         // Iterate through edges
//         for (const auto& edge : edges->second) {
//             if (edge.type == SQUASH) {
//                 // dealloc stack
//                 for (const auto& [type, id] : old_node.allocatedIds | std::views::reverse) {
//                     deallocVar(type, id);
//                 }
//             }
//
//             // Add to queue if not visited
//             auto [child_it, child_inserted] = allocated_nodes.emplace(edge.to_id);
//             if (child_inserted) {
//                 q.push(edge.to_id);
//             }
//         }
//     }
//     return {""};
// }
// void eraxc::JIR::Allocated::AllocationManager::clear() {
//     stack_offsets.clear();
//     used_regs.clear();
//     used_stack_space = 0;
//     allocated_nodes.clear();
// }
// error::errable<void> eraxc::JIR::Allocated::AllocationManager::create(const CFG_Func& f) {
//     // allocate parameters
//     for (const auto& param : f.params) {
//         used_regs.emplace(param.value, pass_ABI[args_in_registers_count++]);
//     }
//     args_in_registers_count = 0;
//     total_stack_space = f.max_stack_size;
//
//     const auto r = allocatedCfgNode(f.node_id);
//     return r;
// }
#include "StackAllocator.hpp"

#include <algorithm>

#include "common/JIR/allocated/Function.hpp"

using namespace eraxc;
using namespace allocation;
using namespace CFG;


error::errable<JIR::allocated::Program> StackAllocator::allocate(const JIR::Program& program) {
    for (const auto& global : program.globals) {
        globals.emplace(global.decl.id);
    }

    std::vector<JIR::allocated::Function> functions;

    for (const auto& function : program.functions) {
        const auto& allocated = allocate(function);
        if (!allocated) {
            return {allocated.error, {{}, {}, 0}};
        }
        functions.emplace_back(allocated.value);
    }

    clear();

    return {"", {functions, program.globals, program.entrypoint_id}};
}

error::errable<JIR::allocated::Function> StackAllocator::allocate(const JIR::Function& function) {
    for (const auto& param : function.params) {
        used_regs.emplace(param.id, pass_ABI[args_in_registers_count++]);
    }
    args_in_registers_count = 0;

    const auto alloc = allocate(function.cfg);
    if (!alloc) {
        return {alloc.error, {}};
    }

    JIR::allocated::Function result {};
    result.cfg = alloc.value;
    result.decl = function.decl;
    result.params = function.params;
    clear();
    return {"", std::move(result)};
}

error::errable<allocated::CFG> StackAllocator::allocate(const CFG::CFG& cfg) {
    allocated::CFG tr;
    tr.edges = cfg.edges;

    tr.nodes.resize(cfg.nodes.size());

    if (cfg.nodes.empty()) {
        return {"", std::move(tr)};
    }

    const auto maxStackSize = cfg.maxStackSize;
    total_stack_space = cfg.maxStackSize;

    CFGIterator<CFG::CFG> iter {cfg, 0};

    while (iter) {

        const auto r = allocateCFGNode(*iter);
        if (!r) {
            return {r.error, {}};
        }

        tr.nodes[iter.nodeId()] = r.value;

        ++iter;
    }

    tr.maxStackSize = maxStackSize;
    clear();
    return {"", std::move(tr)};
}

error::errable<allocated::CFGNode> StackAllocator::allocateCFGNode(const CFGNode& old_node) {
    allocated::CFGNode node;
    for (const auto& alloc : old_node.declarations) {
        allocVar(alloc.type, alloc.id);
    }
    for (const auto& op : old_node.commands) {
        // skip alloc and dealloc
        if (op.op == JIR::Operation::ERR) {
            return {"Encountered ERR operation", std::move(node)};
        }
        if (op.op == JIR::Operation::ALLOC || op.op == JIR::Operation::DEALLOC) {
            continue;
        }

        if (op.op == JIR::Operation::PASS_RET) {
            const auto returnable = operandToAllocated(op.operand1);
            if (!returnable) {
                return {returnable.error, {}};
            }
            node.commands.emplace_back(
                JIR::Operation::MOVE,
                JIR::allocated::Operand {returnable.value.type, (u64)x86_reg::RAX, JIR::allocated::Operand::REGISTER},
                returnable.value);
            continue;
        }

        if (op.op == JIR::Operation::PASS) {
            const auto passed = operandToAllocated(op.operand1);
            if (!passed) {
                return {passed.error, {}};
            }
            node.commands.emplace_back(
                JIR::Operation::MOVE,
                JIR::allocated::Operand {passed.value.type, (u64)pass_ABI[args_in_registers_count++],
                                         JIR::allocated::Operand::REGISTER},
                passed.value);
            continue;
        }

        if (op.op == JIR::Operation::CALL) {
            args_in_registers_count = 0;

            node.commands.emplace_back(
                JIR::Operation::CALL,
                JIR::allocated::Operand {JIR::Type::VOID, op.operand1.value, JIR::allocated::Operand::INSTANT},
                JIR::allocated::Operand {});

            if (op.operand2.type != JIR::Type::VOID) {
                // second operand is a result of call
                auto op2 = operandToAllocated(op.operand2);
                if (!op2) {
                    return {op2.error, {}};
                }
                node.commands.emplace_back(
                    JIR::Operation::MOVE, op2.value,
                    JIR::allocated::Operand {op2.value.type, (u64)x86_reg::RAX, JIR::allocated::Operand::REGISTER});
            }
            continue;
        }

        if (op.op == JIR::Operation::RET) {
            node.commands.emplace_back(
                JIR::Operation::STACKDEALLOC,
                JIR::allocated::Operand {JIR::Type::VOID, used_stack_space, JIR::allocated::Operand::INSTANT},
                JIR::allocated::Operand {});
            node.commands.emplace_back(JIR::Operation::RET, JIR::allocated::Operand {}, JIR::allocated::Operand {});
            continue;
        }


        const auto& operand1 = operandToAllocated(op.operand1);
        if (!operand1) {
            return {operand1.error, {}};
        }
        const auto& operand2 = operandToAllocated(op.operand2);
        if (!operand2) {
            return {operand2.error, {}};
        }
        node.commands.emplace_back(op.op, operand1.value, operand2.value);
    }
    for (const auto& alloc : old_node.declarations) {
        deallocVar(alloc.type, alloc.id);
    }
    return {"", std::move(node)};
}

error::errable<eraxc::JIR::allocated::Operand> StackAllocator::operandToAllocated(const JIR::Operand& operand) {

    if (operand.value == -1 && operand.type == JIR::Type::ERR && operand.is_instant == false &&
        operand.is_rvalue == false) {
        return {"", {}};
    }

    if (operand.is_instant) {
        return error::errable {JIR::allocated::Operand {operand, JIR::allocated::Operand::INSTANT}};
    }

    if (const auto globals_it = globals.find(operand.value); globals_it != globals.end()) {
        // value = global id
        return error::errable {JIR::allocated::Operand {operand, JIR::allocated::Operand::GLOBAL}};
    }

    if (const auto regs_it = used_regs.find(operand.value); regs_it != used_regs.end()) {
        // value = register id
        return error::errable {
            JIR::allocated::Operand {operand.type, (u64)regs_it->second, JIR::allocated::Operand::REGISTER}};
    }

    if (const auto stack_it = stack_offsets.find(operand.value); stack_it != stack_offsets.end()) {
        // value = offset
        return error::errable {
            JIR::allocated::Operand {operand.type, stack_it->second, JIR::allocated::Operand::STACK}};
    }

    return {"Operand $" + std::to_string(operand.value) + " is not allocated", {}};
}
error::errable<void> StackAllocator::deallocVar(JIR::Type type, u64 id) {
    const u64 size = x64::size(type);

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
error::errable<void> StackAllocator::allocVar(JIR::Type type, u64 id) {
    const u64 size = x64::size(type);
    if (stack_offsets.contains(id)) {
        return {"Variable $" + std::to_string(id) + " is already allocated"};
    }
    stack_offsets.emplace(id, total_stack_space);
    total_stack_space -= size;
    if (total_stack_space < 0) {
        return {"Internal error: exceeding stack size"};
    }
    return {""};
}

void StackAllocator::clear() {
    free_regs = all_regs;
    used_regs.clear();
    used_stack_space = 0;
    total_stack_space = 0;
    stack_offsets.clear();
    globals.clear();
}

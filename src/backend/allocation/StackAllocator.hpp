#pragma once
#include "backend/codegen/x64/asm_x64_mem.hpp"
#include "common/CFG/CFGIterator.hpp"
#include "common/CFG/allocated/AllocatedCFG.hpp"
#include "common/JIR/Function.hpp"
#include "common/JIR/Program.hpp"
#include "common/JIR/allocated/Function.hpp"
#include "common/JIR/allocated/Program.hpp"
#include "frontend/syntax/parser/ExpressionParser.hpp"
#include "util/error.hpp"

using namespace eraxc::x64;

namespace eraxc::allocation {

    using RegMap = std::unordered_map<u64, x86_reg>;

    class StackAllocator {

        std::unordered_set<x86_reg> free_regs = x64::all_regs;

        //For mapping used vars to regs
        RegMap used_regs {};

        u64 used_stack_space = 0;
        u64 total_stack_space = 0;

        //For mapping used vars to stack
        std::unordered_map<u64, u64> stack_offsets {};

        std::set<u64> globals {};

        error::errable<JIR::allocated::Operand> operandToAllocated(const JIR::Operand& operand);

        error::errable<void> deallocVar(JIR::Type type, u64 id);

        error::errable<void> allocVar(JIR::Type type, u64 id);

        error::errable<CFG::allocated::CFGNode> allocateCFGNode(const CFG::CFGNode& old_node);

        void clear();

        size_t args_in_registers_count = 0;

    public:
        StackAllocator() = default;

        error::errable<JIR::allocated::Program> allocate(const JIR::Program& program);
        error::errable<CFG::allocated::CFG> allocate(const CFG::CFG& cfg);
        error::errable<JIR::allocated::Function> allocate(const JIR::Function& function);
    };

}

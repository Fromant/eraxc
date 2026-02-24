#pragma once

#include "backend/JIR/CFG/Allocated/CFGAllocated.hpp"
#include "backend/JIR/CFG/Allocated/CFGTypes.hpp"

namespace eraxc::x64 {
    class asm_translator {

        static error::errable<std::string> get_operand(const JIR::Allocated::OperandAllocated& op);

        static error::errable<void> print_JIR_node_asm(const JIR::Allocated::JIRAOp& node, std::ostream& os,
                                                       size_t stackSize);

        static error::errable<void> print_cfg_node(const JIR::Allocated::CFGAllocated& cfg, size_t node_id,
                                                   std::ostream& os, size_t stackSize);

    public:
        static error::errable<void> translate(const JIR::Allocated::CFGAllocated& cfg, const std::string& o_filename);
    };
}

#pragma once
#include "common/JIR/allocated/Command.hpp"
#include "common/JIR/allocated/Program.hpp"
#include "util/error.hpp"

namespace eraxc::x64 {
    class asm_translator {

        static error::errable<std::string> get_operand(const JIR::allocated::Operand& op);

        static error::errable<void> printJirCommand(const JIR::allocated::Command& node, std::ostream& os,
                                                       size_t stackSize);

        static error::errable<void> printFunction(const JIR::allocated::Function& cfg, std::ostream& os,
                                                  size_t stackSize);

    public:
        static error::errable<void> translate(const JIR::allocated::Program& program, const std::string& o_filename);
    };
}

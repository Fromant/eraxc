#pragma once
#include <string>

#include "util/common.hpp"
#include "frontend/syntax/enums.hpp"

namespace eraxc::x64 {

    static constexpr std::string type(u64 type) {
        if (type == syntax::i8 || type == syntax::u8) {
            return "db";
        }
        if (type == syntax::i16 || type == syntax::u16) {
            return "dw";
        }
        if (type == syntax::i32 || type == syntax::u32) {
            return "dd";
        }
        if (type == syntax::i64 || type == syntax::u64) {
            return "dq";
        }
        return {"ILLTYPE"};
    }

    static constexpr u64 size(u64 type) {
        switch (type) {
            case syntax::i8:
            case syntax::u8: return 1;
            case syntax::i16:
            case syntax::u16: return 2;
            case syntax::i32:
            case syntax::u32: return 4;
            case syntax::i64:
            case syntax::u64: return 8;
            default: return -1;
        }
    }

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
    };  //R stands for 64 bits
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
}

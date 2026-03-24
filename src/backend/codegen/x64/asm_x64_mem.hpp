#pragma once
#include <string>

#include "common/JIR/Type.hpp"
#include "frontend/syntax/enums.hpp"
#include "util/common.hpp"

namespace eraxc::x64 {

    static constexpr std::string type(JIR::Type type) {
        if (type == JIR::Type::I8 || type == JIR::Type::U8) {
            return "db";
        }
        if (type == JIR::Type::I16 || type == JIR::Type::U16) {
            return "dw";
        }
        if (type == JIR::Type::I32 || type == JIR::Type::U32) {
            return "dd";
        }
        if (type == JIR::Type::I64 || type == JIR::Type::U64) {
            return "dq";
        }
        return {"ILLTYPE"};
    }

    static constexpr u64 size(JIR::Type type) {
        switch (type) {
            case JIR::Type::VOID: return 0;
            case JIR::Type::I8:
            case JIR::Type::U8: return 1;
            case JIR::Type::I16:
            case JIR::Type::U16: return 2;
            case JIR::Type::I32:
            case JIR::Type::U32: return 4;
            case JIR::Type::I64:
            case JIR::Type::U64: return 8;
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

    static inline std::unordered_set<x86_reg> all_regs {
        x86_reg::RAX, x86_reg::RBX, x86_reg::RCX, x86_reg::RDX, x86_reg::RDI, x86_reg::RSI, x86_reg::R8,
        x86_reg::R9,  x86_reg::R10, x86_reg::R11, x86_reg::R12, x86_reg::R13, x86_reg::R14, x86_reg::R15};

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

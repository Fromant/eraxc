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

    static error::errable<std::string> reg_name(x86_reg reg, u64 type_size) {
        // 8-bit registers
        if (type_size == 1) {
            switch (reg) {
                case x86_reg::RAX: return {"", "al"};
                case x86_reg::RBX: return {"bl", ""};
                case x86_reg::RCX: return {"", "cl"};
                case x86_reg::RDX: return {"", "dl"};
                case x86_reg::RBP: return {"", "bpl"};
                case x86_reg::RSP: return {"", "spl"};
                case x86_reg::RSI: return {"", "sil"};
                case x86_reg::RDI: return {"", "dil"};
                case x86_reg::R8: return {"r8b", ""};
                case x86_reg::R9: return {"r9b", ""};
                case x86_reg::R10: return {"", "r10b"};
                case x86_reg::R11: return {"", "r11b"};
                case x86_reg::R12: return {"", "r12b"};
                case x86_reg::R13: return {"", "r13b"};
                case x86_reg::R14: return {"", "r14b"};
                case x86_reg::R15: return {"", "r15b"};
                default: return {"Unknown register: " + std::to_string((u64)reg), ""};
            }
        }

        // 16-bit registers
        if (type_size == 2) {
            switch (reg) {
                case x86_reg::RAX: return {"", "ax"};
                case x86_reg::RBX: return {"", "bx"};
                case x86_reg::RCX: return {"", "cx"};
                case x86_reg::RDX: return {"", "dx"};
                case x86_reg::RBP: return {"", "bp"};
                case x86_reg::RSP: return {"", "sp"};
                case x86_reg::RSI: return {"", "si"};
                case x86_reg::RDI: return {"", "di"};
                case x86_reg::R8: return {"", "r8w"};
                case x86_reg::R9: return {"", "r9w"};
                case x86_reg::R10: return {"", "r10w"};
                case x86_reg::R11: return {"", "r11w"};
                case x86_reg::R12: return {"", "r12w"};
                case x86_reg::R13: return {"", "r13w"};
                case x86_reg::R14: return {"", "r14w"};
                case x86_reg::R15: return {"", "r15w"};
                default: return {"Unknown register: " + std::to_string((u64)reg), ""};
            }
        }

        // 32-bit registers
        if (type_size == 4) {
            switch (reg) {
                case x86_reg::RAX: return {"", "eax"};
                case x86_reg::RBX: return {"", "ebx"};
                case x86_reg::RCX: return {"", "ecx"};
                case x86_reg::RDX: return {"", "edx"};
                case x86_reg::RBP: return {"", "ebp"};
                case x86_reg::RSP: return {"", "esp"};
                case x86_reg::RSI: return {"", "esi"};
                case x86_reg::RDI: return {"", "edi"};
                case x86_reg::R8: return {"", "r8d"};
                case x86_reg::R9: return {"", "r9d"};
                case x86_reg::R10: return {"", "r10d"};
                case x86_reg::R11: return {"", "r11d"};
                case x86_reg::R12: return {"", "r12d"};
                case x86_reg::R13: return {"", "r13d"};
                case x86_reg::R14: return {"", "r14d"};
                case x86_reg::R15: return {"", "r15d"};
                default: return {"Unknown register: " + std::to_string((u64)reg), ""};
            }
        }

        // 64-bit registers
        if (type_size == 8) {
            switch (reg) {
                case x86_reg::RAX: return {"", "rax"};
                case x86_reg::RBX: return {"", "rbx"};
                case x86_reg::RCX: return {"", "rcx"};
                case x86_reg::RDX: return {"", "rdx"};
                case x86_reg::RBP: return {"", "rbp"};
                case x86_reg::RSP: return {"", "rsp"};
                case x86_reg::RSI: return {"", "rsi"};
                case x86_reg::RDI: return {"", "rdi"};
                case x86_reg::R8: return {"", "r8"};
                case x86_reg::R9: return {"", "r9"};
                case x86_reg::R10: return {"", "r10"};
                case x86_reg::R11: return {"", "r11"};
                case x86_reg::R12: return {"", "r12"};
                case x86_reg::R13: return {"", "r13"};
                case x86_reg::R14: return {"", "r14"};
                case x86_reg::R15: return {"", "r15"};
                default: return {"Unknown register: " + std::to_string((u64)reg), ""};
            }
        }

        return {"Internal error: unknown register size", ""};
    }

    static constexpr x86_reg pass_ABI[] = {x86_reg::RCX, x86_reg::RDX, x86_reg::R8, x86_reg::R9};
}

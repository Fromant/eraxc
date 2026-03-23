#pragma once

#include <string>

namespace eraxc::JIR {
    enum class Type {
        VOID,
        I8,
        U8,
        I16,
        U16,
        I32,
        U32,
        I64,
        U64,

        // FUTURE SECTION
        // PTR, ...

        ERR
    };

    constexpr std::string to_string(const Type& type) {
        if (type == Type::VOID) {
            return "void";
        }
        if (type==Type::I8) {
            return "i8";
        }
        if (type == Type::U8) {
            return "u8";
        }
        if (type==Type::I16) {
            return "i16";
        }
        if (type == Type::U16) {
            return "u16";
        }
        if (type==Type::I32) {
            return "i32";
        }
        if (type == Type::U32) {
            return "u32";
        }
        if (type==Type::I64) {
            return "i64";
        }
        if (type == Type::U64) {
            return "u64";
        }

        return "ERR";
    }

}
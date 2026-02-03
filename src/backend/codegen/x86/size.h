#pragma once

#include <string>

#include "frontend/syntax/enums.h"
#include "util/common.h"

namespace eraxc::x86 {
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
}

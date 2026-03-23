#pragma once

#include "Type.hpp"
#include "util/common.hpp"

namespace eraxc::JIR {
    class Operand {
    public:
        Type type;
        u64 value;
        bool is_instant;
        bool is_rvalue;

        Operand() {
            is_instant = false;
            is_rvalue = false;
            type = Type::ERR;
            value = -1;
        }

        Operand(Type type, u64 value, bool is_instant, bool is_rvalue) :
            type(type), value(value), is_instant(is_instant), is_rvalue(is_rvalue) {}

        std::string to_string() const {
            std::string result {};
            if (type == Type::ERR || value == -1) {
                return "";
            }
            if (!is_instant) {
                result += '$';
            }
            result += std::to_string(value);
            return result;
        }
    };
}
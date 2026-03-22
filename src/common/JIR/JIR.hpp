#pragma once
#include "Operation.hpp"
#include "util/common.hpp"

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

    class Command {
    public:
        Operation op = Operation::ERR;
        Operand operand1;
        Operand operand2;

        void print() const;
    };
}

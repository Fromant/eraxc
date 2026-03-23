#pragma once
#include "Command.hpp"
#include "Type.hpp"
#include "util/common.hpp"

namespace eraxc::JIR {
    struct Declaration {
        u64 id;
        Type type;

        Declaration(const Operand& op) {
            id = op.value;
            type = op.type;
        }

        Declaration(u64 id, Type type) : id(id), type(type) {}

        Declaration() {
            id = -1;
            type = Type::ERR;
        }
    };
}

#pragma once
#include "common/JIR/Operand.hpp"
#include "common/JIR/Operation.hpp"
#include "common/JIR/Type.hpp"
#include "util/common.hpp"

namespace eraxc::JIR::allocated {

    class Operand {
    public:
        enum AllocPlace {
            STACK,
            REGISTER,
            GLOBAL,
            INSTANT
        };

        Operand() = default;
        Operand(Type type, u64 value, AllocPlace place) : type(type), value(value), place {place} {};
        Operand(const JIR::Operand& operand, AllocPlace place) :
            type(operand.type), value(operand.value), place(place) {}
        Type type = Type::ERR;
        u64 value = -1;  // stack offset or register ID or instant value
        AllocPlace place = INSTANT;  // where operand is allocated
    };

    class Command {
    public:
        Command() = default;
        Command(Operation op, const Operand& operand1, const Operand& operand2) :
            op(op), operand1(operand1), operand2 {operand2} {};


        Operation op = Operation::ERR;

        Operand operand1;
        Operand operand2;
    };
}
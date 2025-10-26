
#ifndef OPERATIONS_H
#define OPERATIONS_H

#include "../../frontend/syntax/enums.h"
#include "../../frontend/lexic/preprocessor_tokenizer.h"

namespace eraxc::JIR {

    enum class Operation {
        ADD,
        SUB,
        MUL,
        DIV,
        MOD,

        INC,
        DEC,

        NOT,
        NEG,
        AND,
        OR,
        XOR,
        LSHIFT,
        RSHIFT,

        CMP,

        MOVE,
        CALL,
        RET,
        LABEL,
        JUMP,
        JE,
        JNE,
        JG,
        JGE,
        JL,
        JLE,
        PASS,
        ALLOC,
        DEALLOC,
        STORE,

        NONE,
        ERR
    };

    /// Function to convert general syntax operation to JIR operation
    /// @param op operation to convert from
    /// @return operation::ERR if operation unsupported, operation otherwise
    inline std::pair<Operation, Operation> op_to_jir_op(syntax::operator_type op) {
        if (op == syntax::ADD) return {Operation::ADD, Operation::NONE};
        if (op == syntax::SUBTRACT) return {Operation::SUB, Operation::NONE};
        if (op == syntax::MULTIPLY) return {Operation::MUL, Operation::NONE};
        if (op == syntax::DIVIDE) return {Operation::DIV, Operation::NONE};
        if (op == syntax::MODULO) return {Operation::MOD, Operation::NONE};

        if (op == syntax::INCREMENT) return {Operation::INC, Operation::NONE};
        if (op == syntax::DECREMENT) return {Operation::DEC, Operation::NONE};

        if (op == syntax::AND) return {Operation::AND, Operation::NONE};
        if (op == syntax::OR) return {Operation::OR, Operation::NONE};
        if (op == syntax::NOT) return {Operation::NOT, Operation::NONE};
        if (op == syntax::XOR) return {Operation::XOR, Operation::NONE};
        if (op == syntax::NEGATIVE) return {Operation::NEG, Operation::NONE};

        if (op == syntax::BITWISE_LSHIFT) return {Operation::LSHIFT, Operation::NONE};
        if (op == syntax::BITWISE_RSHIFT) return {Operation::RSHIFT, Operation::NONE};

        if (op == syntax::ASSIGN) return {Operation::MOVE, Operation::NONE};

        if (op == syntax::LESS) return {Operation::CMP, Operation::JL};
        if (op == syntax::LESS_EQ) return {Operation::CMP, Operation::JLE};
        if (op == syntax::GREATER) return {Operation::CMP, Operation::JG};
        if (op == syntax::GREATER_EQ) return {Operation::CMP, Operation::JGE};
        if (op == syntax::EQUAL) return {Operation::CMP, Operation::JNE};
        if (op == syntax::NOT_EQUAL) return {Operation::CMP, Operation::JE};

        // if (op==syntax::ASSIGN) return {Operation::MOVE, Operation::NONE};
        // if (op==syntax::ADD_ASSIGN) return {Operation::ADD, Operation::NONE};
        // if (op==syntax::SUBTRACT_ASSIGN) return {Operation::SUB, Operation::NONE};
        // if (op==syntax::MULT_ASSIGN) return {Operation::MUL, Operation::NONE};
        // if (op==syntax::DIV_ASSIGN) return {Operation::DIV, Operation::NONE};
        // if (op==syntax::MOD_ASSIGN) return {Operation::MOD, Operation::NONE};
        // if (op==syntax::BITWISE_OR_ASSIGN) return {Operation::OR, Operation::NONE};
        // if (op==syntax::BITWISE_AND_ASSIGN) return {Operation::AND, Operation::NONE};
        // if (op==syntax::BITWISE_NOT_ASSIGN) return {Operation::NOT, Operation::NONE};
        // if (op==syntax::BITWISE_XOR_ASSIGN) return {Operation::XOR, Operation::NONE};
        // if (op==syntax::BITWISE_LSHIFT_ASSIGN) return {Operation::LSHIFT, Operation::NONE};
        // if (op==syntax::BITWISE_RSHIFT_ASSIGN) return {Operation::RSHIFT, Operation::NONE};

        return {Operation::ERR, Operation::ERR};
    }

    /// Function to convert general syntax operation to JIR operation
    /// @param t token to convert from
    /// @return operation::ERR if prefix operation unsupported, operation otherwise
    inline Operation prefix_op_to_jir_op(const token& t) {
        if (!syntax::prefix_operators.contains(t.data)) return Operation::ERR;

        auto op = syntax::prefix_operators.at(t.data);

        if (op == syntax::POSITIVE) return Operation::NONE;
        if (op == syntax::NEGATIVE) return Operation::NEG;
        if (op == syntax::NOT || op == syntax::BITWISE_NOT) return Operation::NOT;
        if (op == syntax::INCREMENT) return Operation::INC;
        if (op == syntax::DECREMENT) return Operation::DEC;
        return Operation::ERR;
    }

    inline Operation postfix_op_to_jir_op(const token& t) {
        if (!syntax::postfix_operators.contains(t.data)) return Operation::ERR;
        const auto op = syntax::postfix_operators.at(t.data);
        if (op == syntax::INCREMENT) return Operation::INC;
        if (op == syntax::DECREMENT) return Operation::DEC;
        return Operation::ERR;
    }

    inline Operation assign_op_to_common_op(const syntax::operator_type assign_op) {
        if (assign_op == syntax::operator_type::ADD_ASSIGN) return Operation::ADD;
        if (assign_op == syntax::operator_type::SUBTRACT_ASSIGN) return Operation::SUB;
        if (assign_op == syntax::operator_type::MULT_ASSIGN) return Operation::MUL;
        if (assign_op == syntax::operator_type::DIV_ASSIGN) return Operation::DIV;
        if (assign_op == syntax::operator_type::MOD_ASSIGN) return Operation::MOD;
        if (assign_op == syntax::operator_type::BITWISE_OR_ASSIGN) return Operation::OR;
        if (assign_op == syntax::operator_type::BITWISE_AND_ASSIGN) return Operation::AND;
        if (assign_op == syntax::operator_type::BITWISE_NOT_ASSIGN) return Operation::NOT;
        if (assign_op == syntax::operator_type::BITWISE_XOR_ASSIGN) return Operation::XOR;
        if (assign_op == syntax::operator_type::BITWISE_LSHIFT_ASSIGN) return Operation::LSHIFT;
        if (assign_op == syntax::operator_type::BITWISE_RSHIFT_ASSIGN) return Operation::RSHIFT;
        if (assign_op == syntax::operator_type::ASSIGN) return Operation::MOVE;
        return Operation::ERR;
    }

}

#endif  //OPERATIONS_H

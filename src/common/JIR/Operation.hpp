#pragma once
#include <string>

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
        PASS_RET,
        ALLOC,
        DEALLOC,
        STORE,

        NONE,
        ERR
    };


    constexpr Operation getInvertedJump(Operation jump) {
        if (jump == Operation::JE)
            return Operation::JGE;
        if (jump == Operation::JLE)
            return Operation::JG;
        if (jump == Operation::JGE)
            return Operation::JL;
        if (jump == Operation::JG)
            return Operation::JLE;

        return Operation::ERR;
    }

    inline std::string operationToString(const Operation& node) {

        if (node == Operation::ADD) {
            return "ADD";
        }
        if (node == Operation::SUB) {
            return "SUB";
        }
        if (node == Operation::MUL) {
            return "MUL";
        }
        if (node == Operation::DIV) {
            return "DIV";
        }
        if (node == Operation::MOD) {
            return "MOD";
        }


        if (node == Operation::INC) {
            return "INC";
        }
        if (node == Operation::DEC) {
            return "DEC";
        }


        if (node == Operation::NOT) {
            return "NOT";
        }
        if (node == Operation::NEG) {
            return "NEG";
        }
        if (node == Operation::AND) {
            return "AND";
        }
        if (node == Operation::OR) {
            return "OR";
        }
        if (node == Operation::XOR) {
            return "XOR";
        }
        if (node == Operation::LSHIFT) {
            return "LSHIFT";
        }
        if (node == Operation::RSHIFT) {
            return "RSHIFT";
        }


        if (node == Operation::CMP) {
            return "CMP";
        }


        if (node == Operation::MOVE) {
            return "MOVE";
        }
        if (node == Operation::CALL) {
            return "CALL $f_";
        }
        if (node == Operation::RET) {
            return "RET";
        }
        if (node == Operation::LABEL) {
            return ".l";
        }
        if (node == Operation::JUMP) {
            return "JUMP .l";
        }
        if (node == Operation::JE) {
            return "JE .l";
        }
        if (node == Operation::JNE) {
            return "JNE .l";
        }
        if (node == Operation::JL) {
            return "JL .l";
        }
        if (node == Operation::JLE) {
            return "JLE .l";
        }
        if (node == Operation::JG) {
            return "JG .l";
        }
        if (node == Operation::JGE) {
            return "JGE .l";
        }


        if (node == Operation::PASS) {
            return "PASS";
        }
        if (node == Operation::PASS_RET) {
            return "PASS RET";
        }
        if (node == Operation::ALLOC) {
            return "ALLOC";
        }
        if (node == Operation::DEALLOC) {
            return "DEALLOC";
        }

        if (node == Operation::STORE) {
            return "STORE";
        }

        if (node == Operation::NONE) {
            return "NONE";
        }

        return "ERR";
    }
}

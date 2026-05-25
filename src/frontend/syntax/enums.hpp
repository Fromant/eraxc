#pragma once

#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "common/JIR/Command.hpp"
#include "common/JIR/Operation.hpp"
#include "frontend/lexic/PreprocessorTokenizer.hpp"
#include "util/error.hpp"

namespace eraxc::frontend {
    enum class OperatorType : unsigned char {
        //arithmetic
        ADD,
        SUBTRACT,
        MULTIPLY,
        DIVIDE,
        MODULO,

        //compare
        EQUAL,
        NOT_EQUAL,
        GREATER,
        GREATER_EQ,
        LESS,
        LESS_EQ,

        //logic
        AND,
        OR,
        NOT,
        XOR,
        //bitwise
        BITWISE_NOT,
        BITWISE_AND,
        BITWISE_OR,
        BITWISE_XOR,
        BITWISE_LSHIFT,
        BITWISE_RSHIFT,

        //assign
        ASSIGN,
        ADD_ASSIGN,
        SUBTRACT_ASSIGN,
        MULT_ASSIGN,
        DIV_ASSIGN,
        MOD_ASSIGN,
        BITWISE_NOT_ASSIGN,
        BITWISE_AND_ASSIGN,
        BITWISE_OR_ASSIGN,
        BITWISE_XOR_ASSIGN,
        BITWISE_LSHIFT_ASSIGN,
        BITWISE_RSHIFT_ASSIGN,

        //unary and postfix
        POSITIVE,
        NEGATIVE,
        ADDRESSOF,
        INDIRECTION,
        INCREMENT,
        DECREMENT,

        //stack management
        ALLOC,
        DEALLOC,

        //call operator
        CALL,
        NONE
    };

    enum class Keyword : unsigned char {
        i8,
        i16,
        i32,
        i64,
        i128,
        i256,
        u8,
        u16,
        u32,
        u64,
        u128,
        u256,
        INT,
        LONG,
        CHAR,
        BOOL,
        SHORT,
        BYTE,
        AUTO,
        VOID,
        TRUE,
        FALSE,
        ENUM,
        STRUCT,
        THIS,
        PUBLIC,
        PRIVATE,
        PROTECTED,
        UNION,
        CONST,
        STATIC,
        OPERATOR,
        NEW,
        DELETE,
        DEFER,
        SIZEOF,
        ALIGNAS,
        ASM,
        FOR,
        WHILE,
        DO,
        BREAK,
        CONTINUE,
        RETURN,
        NAMESPACE
    };

    static constexpr JIR::Type jirTypeFromKeyword(const Keyword k) {
        if (k == Keyword::i8 || k == Keyword::CHAR) {
            return JIR::Type::I8;
        }
        if (k == Keyword::u8) {
            return JIR::Type::U8;
        }

        if (k == Keyword::SHORT || k == Keyword::i16) {
            return JIR::Type::I16;
        }
        if (k == Keyword::u16) {
            return JIR::Type::U16;
        }

        if (k == Keyword::INT || k == Keyword::i32) {
            return JIR::Type::I32;
        }
        if (k == Keyword::u32) {
            return JIR::Type::U32;
        }

        if (k == Keyword::LONG || k == Keyword::i64) {
            return JIR::Type::I64;
        }
        if (k == Keyword::u64) {
            return JIR::Type::U64;
        }

        return JIR::Type::ERR;
    }

    static constexpr Keyword keywordFromJirType(const JIR::Type t) {
        if (t == JIR::Type::I8) {
            return Keyword::i8;
        }
        if (t == JIR::Type::U8) {
            return Keyword::u8;
        }

        if (t == JIR::Type::I16) {
            return Keyword::i16;
        }
        if (t == JIR::Type::U16) {
            return Keyword::u16;
        }

        if (t == JIR::Type::I32) {
            return Keyword::i32;
        }
        if (t == JIR::Type::U32) {
            return Keyword::u32;
        }
        if (t == JIR::Type::I64) {
            return Keyword::i64;
        }
        if (t == JIR::Type::U64) {
            return Keyword::u64;
        }

        return Keyword::VOID;
    }

    static const inline std::unordered_map<std::string, Keyword> keywords {{"i8", Keyword::i8},
                                                                           {"i16", Keyword::i16},
                                                                           {"i32", Keyword::i32},
                                                                           {"i64", Keyword::i64},
                                                                           {"i128", Keyword::i128},
                                                                           {"i256", Keyword::i256},

                                                                           {"u8", Keyword::u8},
                                                                           {"u16", Keyword::u16},
                                                                           {"u32", Keyword::u32},
                                                                           {"u64", Keyword::u64},
                                                                           {"u128", Keyword::u128},
                                                                           {"u256", Keyword::u256},

                                                                           {"int", Keyword::INT},
                                                                           {"long", Keyword::LONG},
                                                                           {"char", Keyword::CHAR},
                                                                           {"bool", Keyword::BOOL},
                                                                           {"short", Keyword::SHORT},
                                                                           {"byte", Keyword::BYTE},

                                                                           {"auto", Keyword::AUTO},
                                                                           {"void", Keyword::VOID},

                                                                           {"true", Keyword::TRUE},
                                                                           {"false", Keyword::FALSE},

                                                                           {"enum", Keyword::ENUM},
                                                                           {"namespace", Keyword::NAMESPACE},
                                                                           {"struct", Keyword::STRUCT},
                                                                           {"this", Keyword::THIS},
                                                                           {"public", Keyword::PUBLIC},
                                                                           {"private", Keyword::PRIVATE},
                                                                           {"protected", Keyword::PROTECTED},
                                                                           {"union", Keyword::UNION},

                                                                           {"const", Keyword::CONST},
                                                                           {"static", Keyword::STATIC},
                                                                           {"operator", Keyword::OPERATOR},
                                                                           {"return", Keyword::RETURN},

                                                                           {"new", Keyword::NEW},
                                                                           {"delete", Keyword::DELETE},
                                                                           {"defer", Keyword::DEFER},
                                                                           {"sizeof", Keyword::SIZEOF},
                                                                           {"alignas", Keyword::ALIGNAS},

                                                                           {"asm", Keyword::ASM},

                                                                           {"for", Keyword::FOR},
                                                                           {"while", Keyword::WHILE},
                                                                           {"do", Keyword::DO},
                                                                           {"break", Keyword::BREAK},
                                                                           {"continue", Keyword::CONTINUE}};

    static error::errable<size_t> type_size(const Keyword keyword) {
        switch (keyword) {
            case Keyword::u8:
            case Keyword::i8: return {"", 1};
            case Keyword::u16:
            case Keyword::i16: return {"", 2};
            case Keyword::u32:
            case Keyword::i32: return {"", 4};
            case Keyword::u64:
            case Keyword::i64: return {"", 8};
            default:
                for (const auto& [name, key] : keywords) {
                    if (keyword == key) {
                        return {"Unknown type size: " + name, 0};
                    }
                }
                return {"Unknown type size, keyword not found: " + std::to_string((size_t)keyword), 0};
        }
    }

    static const inline std::unordered_set<OperatorType> compare_operators = {
        OperatorType::EQUAL,      OperatorType::NOT_EQUAL, OperatorType::GREATER,
        OperatorType::GREATER_EQ, OperatorType::LESS,      OperatorType::LESS_EQ};

    static const inline std::unordered_map<std::string, OperatorType> operators {
        {"==", OperatorType::EQUAL},
        {"!=", OperatorType::NOT_EQUAL},
        {"=", OperatorType::ASSIGN},
        {">", OperatorType::GREATER},
        {"<", OperatorType::LESS},
        {">=", OperatorType::GREATER_EQ},
        {"<=", OperatorType::LESS_EQ},
        {"+", OperatorType::ADD},
        {"-", OperatorType::SUBTRACT},
        {"*", OperatorType::MULTIPLY},
        {"/", OperatorType::DIVIDE},
        {"%", OperatorType::MODULO},
        {"&&", OperatorType::AND},
        {"||", OperatorType::OR},
        {"^^", OperatorType::XOR},
        {"!", OperatorType::NOT},
        {"&", OperatorType::BITWISE_AND},
        {"|", OperatorType::BITWISE_OR},
        {"^", OperatorType::BITWISE_XOR},
        {"~", OperatorType::BITWISE_NOT},
        {">>", OperatorType::BITWISE_RSHIFT},
        {"<<", OperatorType::BITWISE_LSHIFT},
        {"+=", OperatorType::ADD_ASSIGN},
        {"-=", OperatorType::SUBTRACT_ASSIGN},
        {"*=", OperatorType::MULT_ASSIGN},
        {"/=", OperatorType::DIV_ASSIGN},
        {"%=", OperatorType::MOD_ASSIGN},
        {"&=", OperatorType::BITWISE_AND_ASSIGN},
        {"^=", OperatorType::BITWISE_XOR_ASSIGN},
        {"|=", OperatorType::BITWISE_OR_ASSIGN},
        {"~=", OperatorType::BITWISE_NOT_ASSIGN},
        {">>=", OperatorType::BITWISE_RSHIFT_ASSIGN},
        {"<<=", OperatorType::BITWISE_LSHIFT_ASSIGN},

        //prefix and postfix operators
        {"++", OperatorType::INCREMENT},
        {"--", OperatorType::DECREMENT},
        {"+", OperatorType::POSITIVE},
        {"-", OperatorType::NEGATIVE},
        {"*", OperatorType::INDIRECTION},
        {"&", OperatorType::ADDRESSOF},
        {"!", OperatorType::NOT},
        {"~", OperatorType::BITWISE_NOT},
    };

    const inline std::set<OperatorType> assign_operators {OperatorType::ASSIGN,
                                                          OperatorType::ADD_ASSIGN,
                                                          OperatorType::SUBTRACT_ASSIGN,
                                                          OperatorType::MULT_ASSIGN,
                                                          OperatorType::DIV_ASSIGN,
                                                          OperatorType::MOD_ASSIGN,
                                                          OperatorType::BITWISE_AND_ASSIGN,
                                                          OperatorType::BITWISE_XOR_ASSIGN,
                                                          OperatorType::BITWISE_OR_ASSIGN,
                                                          OperatorType::BITWISE_NOT_ASSIGN,
                                                          OperatorType::BITWISE_LSHIFT_ASSIGN,
                                                          OperatorType::BITWISE_RSHIFT_ASSIGN};

    const static inline std::unordered_map<OperatorType, OperatorType> assign_to_common_op {
        {OperatorType::ADD_ASSIGN, OperatorType::ADD},
        {OperatorType::SUBTRACT_ASSIGN, OperatorType::SUBTRACT},
        {OperatorType::MULT_ASSIGN, OperatorType::MULTIPLY},
        {OperatorType::DIV_ASSIGN, OperatorType::DIVIDE},
        {OperatorType::MOD_ASSIGN, OperatorType::MODULO},
        {OperatorType::BITWISE_AND_ASSIGN, OperatorType::BITWISE_AND},
        {OperatorType::BITWISE_XOR_ASSIGN, OperatorType::BITWISE_XOR},
        {OperatorType::BITWISE_OR_ASSIGN, OperatorType::BITWISE_OR},
        {OperatorType::BITWISE_NOT_ASSIGN, OperatorType::BITWISE_NOT},
        {OperatorType::BITWISE_LSHIFT_ASSIGN, OperatorType::BITWISE_LSHIFT},
        {OperatorType::BITWISE_RSHIFT_ASSIGN, OperatorType::BITWISE_RSHIFT},
        {OperatorType::ASSIGN, OperatorType::ASSIGN}};

    //less = more priority
    const static inline std::unordered_map<OperatorType, int> operator_priorities {
        {OperatorType::MULTIPLY, 5},
        {OperatorType::DIVIDE, 5},
        {OperatorType::MODULO, 5},

        {OperatorType::ADD, 6},
        {OperatorType::SUBTRACT, 6},

        {OperatorType::BITWISE_RSHIFT, 7},
        {OperatorType::BITWISE_LSHIFT, 7},

        {OperatorType::LESS, 9},
        {OperatorType::LESS_EQ, 9},
        {OperatorType::GREATER, 9},
        {OperatorType::GREATER_EQ, 9},

        {OperatorType::EQUAL, 10},
        {OperatorType::NOT_EQUAL, 10},

        {OperatorType::BITWISE_AND, 11},
        {OperatorType::BITWISE_XOR, 12},
        {OperatorType::BITWISE_OR, 13},

        {OperatorType::AND, 14},
        {OperatorType::OR, 15},
        {OperatorType::XOR, 10},  //same as NOT_EQUAL


        {OperatorType::ASSIGN, 16},
        {OperatorType::ADD_ASSIGN, 16},
        {OperatorType::SUBTRACT_ASSIGN, 16},
        {OperatorType::MULT_ASSIGN, 16},
        {OperatorType::DIV_ASSIGN, 16},
        {OperatorType::MOD_ASSIGN, 16},
        {OperatorType::BITWISE_LSHIFT_ASSIGN, 16},
        {OperatorType::BITWISE_RSHIFT_ASSIGN, 16},
        {OperatorType::BITWISE_AND_ASSIGN, 16},
        {OperatorType::BITWISE_OR_ASSIGN, 16},
        {OperatorType::BITWISE_XOR_ASSIGN, 16}};

    const inline std::unordered_map<std::string, OperatorType> prefix_operators {
        {"+", OperatorType::POSITIVE},   {"-", OperatorType::NEGATIVE},  {"*", OperatorType::INDIRECTION},
        {"&", OperatorType::ADDRESSOF},  {"!", OperatorType::NOT},       {"~", OperatorType::BITWISE_NOT},
        {"++", OperatorType::INCREMENT}, {"--", OperatorType::DECREMENT}};

    const inline std::unordered_map<std::string, OperatorType> postfix_operators {
        {"++", OperatorType::INCREMENT},
        {"--", OperatorType::DECREMENT},
    };


    /// Function to convert general syntax operation to JIR operation
    /// @param op operation to convert from
    /// @return operation::ERR if operation unsupported, operation otherwise
    inline JIR::Operation opToJirOp(OperatorType op) {
        if (op == OperatorType::ADD) {
            return JIR::Operation::ADD;
        }
        if (op == OperatorType::SUBTRACT) {
            return JIR::Operation::SUB;
        }
        if (op == OperatorType::MULTIPLY) {
            return JIR::Operation::MUL;
        }
        if (op == OperatorType::DIVIDE) {
            return JIR::Operation::DIV;
        }
        if (op == OperatorType::MODULO) {
            return JIR::Operation::MOD;
        }

        if (op == OperatorType::INCREMENT) {
            return JIR::Operation::INC;
        }
        if (op == OperatorType::DECREMENT) {
            return JIR::Operation::DEC;
        }

        if (op == OperatorType::AND) {
            return JIR::Operation::AND;
        }
        if (op == OperatorType::OR) {
            return JIR::Operation::OR;
        }
        if (op == OperatorType::NOT) {
            return JIR::Operation::NOT;
        }
        if (op == OperatorType::XOR) {
            return JIR::Operation::XOR;
        }
        if (op == OperatorType::NEGATIVE) {
            return JIR::Operation::NEG;
        }

        if (op == OperatorType::BITWISE_AND) {
            return JIR::Operation::AND;
        }
        if (op == OperatorType::BITWISE_OR) {
            return JIR::Operation::OR;
        }
        if (op == OperatorType::BITWISE_NOT) {
            return JIR::Operation::NOT;
        }
        if (op == OperatorType::BITWISE_XOR) {
            return JIR::Operation::XOR;
        }

        if (op == OperatorType::BITWISE_LSHIFT) {
            return JIR::Operation::LSHIFT;
        }
        if (op == OperatorType::BITWISE_RSHIFT) {
            return JIR::Operation::RSHIFT;
        }

        if (op == OperatorType::ASSIGN) {
            return JIR::Operation::MOVE;
        }

        if (op == OperatorType::LESS) {
            return JIR::Operation::CMP;
        }
        if (op == OperatorType::GREATER) {
            return JIR::Operation::CMP;
        }
        if (op == OperatorType::LESS_EQ) {
            return JIR::Operation::CMP;
        }
        if (op == OperatorType::GREATER_EQ) {
            return JIR::Operation::CMP;
        }
        if (op == OperatorType::EQUAL) {
            return JIR::Operation::CMP;
        }
        if (op == OperatorType::NOT_EQUAL) {
            return JIR::Operation::CMP;
        }

        return JIR::Operation::ERR;
    }

    inline JIR::Operation cmpOpToJirOp(OperatorType op) {
        if (op == OperatorType::LESS) {
            return JIR::Operation::JL;
        }
        if (op == OperatorType::LESS_EQ) {
            return JIR::Operation::JLE;
        }
        if (op == OperatorType::GREATER) {
            return JIR::Operation::JG;
        }
        if (op == OperatorType::GREATER_EQ) {
            return JIR::Operation::JGE;
        }
        if (op == OperatorType::EQUAL) {
            return JIR::Operation::JE;
        }
        if (op == OperatorType::NOT_EQUAL) {
            return JIR::Operation::JNE;
        }
        return JIR::Operation::ERR;
    }

    /// Function to convert general OperatorType operation to JIR operation
    /// @param t token to convert from
    /// @return operation::ERR if prefix operation unsupported, operation otherwise
    inline JIR::Operation prefixOpToJirOp(const Token& t) {
        if (!prefix_operators.contains(t.data)) {
            return JIR::Operation::ERR;
        }

        auto op = prefix_operators.at(t.data);

        if (op == OperatorType::POSITIVE)
            return JIR::Operation::NONE;
        if (op == OperatorType::NEGATIVE)
            return JIR::Operation::NEG;
        if (op == OperatorType::NOT || op == OperatorType::BITWISE_NOT)
            return JIR::Operation::NOT;
        if (op == OperatorType::INCREMENT)
            return JIR::Operation::INC;
        if (op == OperatorType::DECREMENT)
            return JIR::Operation::DEC;
        if (op == OperatorType::INDIRECTION)
            return JIR::Operation::DEREF;
        if (op == OperatorType::ADDRESSOF)
            return JIR::Operation::ADDR;
        return JIR::Operation::ERR;
    }

    inline JIR::Operation postfixOpToJirOp(const Token& t) {
        if (!postfix_operators.contains(t.data))
            return JIR::Operation::ERR;
        const auto op = postfix_operators.at(t.data);
        if (op == OperatorType::INCREMENT)
            return JIR::Operation::INC;
        if (op == OperatorType::DECREMENT)
            return JIR::Operation::DEC;
        return JIR::Operation::ERR;
    }

    inline JIR::Operation assignOpToCommonOp(const OperatorType assign_op) {
        if (assign_op == OperatorType::ADD_ASSIGN)
            return JIR::Operation::ADD;
        if (assign_op == OperatorType::SUBTRACT_ASSIGN)
            return JIR::Operation::SUB;
        if (assign_op == OperatorType::MULT_ASSIGN)
            return JIR::Operation::MUL;
        if (assign_op == OperatorType::DIV_ASSIGN)
            return JIR::Operation::DIV;
        if (assign_op == OperatorType::MOD_ASSIGN)
            return JIR::Operation::MOD;
        if (assign_op == OperatorType::BITWISE_OR_ASSIGN)
            return JIR::Operation::OR;
        if (assign_op == OperatorType::BITWISE_AND_ASSIGN)
            return JIR::Operation::AND;
        if (assign_op == OperatorType::BITWISE_NOT_ASSIGN)
            return JIR::Operation::NOT;
        if (assign_op == OperatorType::BITWISE_XOR_ASSIGN)
            return JIR::Operation::XOR;
        if (assign_op == OperatorType::BITWISE_LSHIFT_ASSIGN)
            return JIR::Operation::LSHIFT;
        if (assign_op == OperatorType::BITWISE_RSHIFT_ASSIGN)
            return JIR::Operation::RSHIFT;
        if (assign_op == OperatorType::ASSIGN)
            return JIR::Operation::MOVE;
        return JIR::Operation::ERR;
    }

    static const inline std::unordered_set<OperatorType> cond_operators = {
        OperatorType::EQUAL,      OperatorType::NOT_EQUAL, OperatorType::GREATER,
        OperatorType::GREATER_EQ, OperatorType::LESS,      OperatorType::LESS_EQ};

    static constexpr JIR::BooleanOperation conditionalOpToBooleanOperation(const OperatorType& op) {
        if (op == OperatorType::EQUAL) {
            return JIR::BooleanOperation::EQUAL;
        }
        if (op == OperatorType::NOT_EQUAL) {
            return JIR::BooleanOperation::NOT_EQUAL;
        }
        if (op == OperatorType::GREATER) {
            return JIR::BooleanOperation::GREATER;
        }
        if (op == OperatorType::GREATER_EQ) {
            return JIR::BooleanOperation::GREATER_EQUAL;
        }
        if (op == OperatorType::LESS) {
            return JIR::BooleanOperation::LESS;
        }
        if (op == OperatorType::LESS_EQ) {
            return JIR::BooleanOperation::LESS_EQUAL;
        }
        return JIR::BooleanOperation::ERR;
    }

}

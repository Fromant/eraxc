#ifndef ERAXC_ENUMS_H
#define ERAXC_ENUMS_H

#include <string>
#include <unordered_map>

namespace eraxc::syntax {

    inline constexpr std::string NOT_FOUND{"NOT_FOUND"};

    enum operator_type : unsigned char {
        //arithmetic
        ADD,  //+
        SUBTRACT, //-
        MULTIPLY, //*
        DIVIDE, // /
        MODULO, //%

        //compare
        EQUAL, NOT_EQUAL,
        GREATER, GREATER_EQ,
        LESS, LESS_EQ,

        //logic
        AND, OR, NOT, XOR,
        //bitwise
        BITWISE_NOT, BITWISE_AND, BITWISE_OR, BITWISE_XOR, BITWISE_LSHIFT, BITWISE_RSHIFT,

        //assign
        ASSIGN, ADD_ASSIGN, SUBTRACT_ASSIGN, MULT_ASSIGN, DIV_ASSIGN, MOD_ASSIGN,
        BITWISE_NOT_ASSIGN, BITWISE_AND_ASSIGN, BITWISE_OR_ASSIGN,
        BITWISE_XOR_ASSIGN, BITWISE_LSHIFT_ASSIGN, BITWISE_RSHIFT_ASSIGN,

        //unary and postfix
        POSITIVE, NEGATIVE, ADDRESSOF, INDIRECTION, INCREMENT, DECREMENT,

        //call operator
        CALL,
        NONE
    };

    enum Keyword : unsigned char {
        i8, i16, i32, i64, i128, i256,
        u8, u16, u32, u64, u128, u256,
        INT, LONG, CHAR, BOOL, SHORT, BYTE,
        AUTO, VOID,
        TRUE, FALSE,

        ENUM, STRUCT,
        THIS,
        PUBLIC, PRIVATE, PROTECTED,
        UNION,

        CONST,
        STATIC,
        OPERATOR,
        NEW, DELETE,
        DEFER,
        SIZEOF,
        ALIGNAS,
        ASM,
        FOR, WHILE, DO, BREAK, CONTINUE,
        RETURN,
        NAMESPACE
    };

    static const inline std::unordered_map<std::string, Keyword> keywords{
            {"i8",        i8},
            {"i16",       i16},
            {"i32",       i32},
            {"i64",       i64},
            {"i128",      i128},
            {"i256",      i256},

            {"u8",        u8},
            {"u16",       u16},
            {"u32",       u32},
            {"u64",       u64},
            {"u128",      u128},
            {"u256",      u256},

            {"int",       INT},
            {"long",      LONG},
            {"char",      CHAR},
            {"bool",      BOOL},
            {"short",     SHORT},

            {"auto",      AUTO},
            {"void",      VOID},

            {"true",      TRUE},
            {"false",     FALSE},

            {"enum",      ENUM},
            {"namespace", NAMESPACE},
            {"struct",    STRUCT},
            {"this",      THIS},
            {"public",    PUBLIC},
            {"private",   PRIVATE},
            {"protected", PROTECTED},
            {"union",     UNION},

            {"const",     CONST},
            {"static",    STATIC},
            {"operator",  OPERATOR},
            {"return",    RETURN},

            {"new",       NEW},
            {"delete",    DELETE},
            {"defer",     DEFER},
            {"sizeof",    SIZEOF},
            {"alignas",   ALIGNAS},

            {"asm",       ASM},

            {"for",       FOR},
            {"while",     WHILE},
            {"do",        DO},
            {"break",     BREAK},
            {"continue",  CONTINUE}
    };

    static const inline std::unordered_map<std::string, operator_type> operators{
            {"==",  EQUAL},
            {"!=",  NOT_EQUAL},
            {"=",   ASSIGN},
            {">",   GREATER},
            {"<",   LESS},
            {">=",  GREATER_EQ},
            {"<=",  LESS_EQ},
            {"+",   ADD},
            {"-",   SUBTRACT},
            {"*",   MULTIPLY},
            {"/",   DIVIDE},
            {"%",   MODULO},
            {"&&",  AND},
            {"||",  OR},
            {"^^",  XOR},
            {"!",   NOT},
            {"&",   BITWISE_AND},
            {"|",   BITWISE_OR},
            {"^",   BITWISE_XOR},
            {"~",   BITWISE_NOT},
            {">>",  BITWISE_RSHIFT},
            {"<<",  BITWISE_LSHIFT},
            {"+=",  ADD_ASSIGN},
            {"-=",  SUBTRACT_ASSIGN},
            {"*=",  MULT_ASSIGN},
            {"/=",  DIV_ASSIGN},
            {"%=",  MOD_ASSIGN},
            {"&=",  BITWISE_AND_ASSIGN},
            {"^=",  BITWISE_XOR_ASSIGN},
            {"|=",  BITWISE_OR_ASSIGN},
            {"~=",  BITWISE_NOT_ASSIGN},
            {">>=", BITWISE_RSHIFT_ASSIGN},
            {"<<=", BITWISE_LSHIFT_ASSIGN}
    };

    const inline std::unordered_map<std::string, operator_type> unary_operators{
            {"+",  POSITIVE},
            {"-",  NEGATIVE},
            {"*",  INDIRECTION},
            {"&",  ADDRESSOF},
            {"!",  NOT},
            {"~",  BITWISE_NOT},
            {"++", INCREMENT},
            {"--", DECREMENT}
    };

    const inline std::unordered_map<std::string, operator_type> postfix_operators{
            {"++", INCREMENT},
            {"--", DECREMENT},
    };

    const std::string &find_op(eraxc::syntax::operator_type t);
}

#endif //ERAXC_ENUMS_H

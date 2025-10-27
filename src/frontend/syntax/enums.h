#pragma once

#include <set>
#include <string>
#include <unordered_map>

namespace eraxc::syntax {
    enum operator_type : unsigned char {
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

        //call operator
        CALL,
        NONE
    };

    enum Keyword : unsigned char {
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

    static const inline std::unordered_map<std::string, Keyword> keywords {{"i8", i8},
                                                                           {"i16", i16},
                                                                           {"i32", i32},
                                                                           {"i64", i64},
                                                                           {"i128", i128},
                                                                           {"i256", i256},

                                                                           {"u8", u8},
                                                                           {"u16", u16},
                                                                           {"u32", u32},
                                                                           {"u64", u64},
                                                                           {"u128", u128},
                                                                           {"u256", u256},

                                                                           {"int", INT},
                                                                           {"long", LONG},
                                                                           {"char", CHAR},
                                                                           {"bool", BOOL},
                                                                           {"short", SHORT},
                                                                           {"byte", BYTE},

                                                                           {"auto", AUTO},
                                                                           {"void", VOID},

                                                                           {"true", TRUE},
                                                                           {"false", FALSE},

                                                                           {"enum", ENUM},
                                                                           {"namespace", NAMESPACE},
                                                                           {"struct", STRUCT},
                                                                           {"this", THIS},
                                                                           {"public", PUBLIC},
                                                                           {"private", PRIVATE},
                                                                           {"protected", PROTECTED},
                                                                           {"union", UNION},

                                                                           {"const", CONST},
                                                                           {"static", STATIC},
                                                                           {"operator", OPERATOR},
                                                                           {"return", RETURN},

                                                                           {"new", NEW},
                                                                           {"delete", DELETE},
                                                                           {"defer", DEFER},
                                                                           {"sizeof", SIZEOF},
                                                                           {"alignas", ALIGNAS},

                                                                           {"asm", ASM},

                                                                           {"for", FOR},
                                                                           {"while", WHILE},
                                                                           {"do", DO},
                                                                           {"break", BREAK},
                                                                           {"continue", CONTINUE}};

    static const inline std::unordered_map<std::string, operator_type> operators {
        {"==", EQUAL},
        {"!=", NOT_EQUAL},
        {"=", ASSIGN},
        {">", GREATER},
        {"<", LESS},
        {">=", GREATER_EQ},
        {"<=", LESS_EQ},
        {"+", ADD},
        {"-", SUBTRACT},
        {"*", MULTIPLY},
        {"/", DIVIDE},
        {"%", MODULO},
        {"&&", AND},
        {"||", OR},
        {"^^", XOR},
        {"!", NOT},
        {"&", BITWISE_AND},
        {"|", BITWISE_OR},
        {"^", BITWISE_XOR},
        {"~", BITWISE_NOT},
        {">>", BITWISE_RSHIFT},
        {"<<", BITWISE_LSHIFT},
        {"+=", ADD_ASSIGN},
        {"-=", SUBTRACT_ASSIGN},
        {"*=", MULT_ASSIGN},
        {"/=", DIV_ASSIGN},
        {"%=", MOD_ASSIGN},
        {"&=", BITWISE_AND_ASSIGN},
        {"^=", BITWISE_XOR_ASSIGN},
        {"|=", BITWISE_OR_ASSIGN},
        {"~=", BITWISE_NOT_ASSIGN},
        {">>=", BITWISE_RSHIFT_ASSIGN},
        {"<<=", BITWISE_LSHIFT_ASSIGN},

        //prefix and postfix operators
        {"++", INCREMENT},
        {"--", DECREMENT},
        {"+", POSITIVE},
        {"-", NEGATIVE},
        {"*", INDIRECTION},
        {"&", ADDRESSOF},
        {"!", NOT},
        {"~", BITWISE_NOT},
    };

    const inline std::set<operator_type> assign_operators {ASSIGN,
                                                           ADD_ASSIGN,
                                                           SUBTRACT_ASSIGN,
                                                           MULT_ASSIGN,
                                                           DIV_ASSIGN,
                                                           MOD_ASSIGN,
                                                           BITWISE_AND_ASSIGN,
                                                           BITWISE_XOR_ASSIGN,
                                                           BITWISE_OR_ASSIGN,
                                                           BITWISE_NOT_ASSIGN,
                                                           BITWISE_LSHIFT_ASSIGN,
                                                           BITWISE_RSHIFT_ASSIGN};

    const static inline std::unordered_map<operator_type, operator_type> assign_to_common_op {
        {ADD_ASSIGN, ADD},
        {SUBTRACT_ASSIGN, SUBTRACT},
        {MULT_ASSIGN, MULTIPLY},
        {DIV_ASSIGN, DIVIDE},
        {MOD_ASSIGN, MODULO},
        {BITWISE_AND_ASSIGN, BITWISE_AND},
        {BITWISE_XOR_ASSIGN, BITWISE_XOR},
        {BITWISE_OR_ASSIGN, BITWISE_OR},
        {BITWISE_NOT_ASSIGN, BITWISE_NOT},
        {BITWISE_LSHIFT_ASSIGN, BITWISE_LSHIFT},
        {BITWISE_RSHIFT_ASSIGN, BITWISE_RSHIFT},
        {ASSIGN, ASSIGN}};

    //less = more priority
    const static inline std::unordered_map<operator_type, int> operator_priorities {{MULTIPLY, 5},
                                                                                    {DIVIDE, 5},
                                                                                    {MODULO, 5},

                                                                                    {ADD, 6},
                                                                                    {SUBTRACT, 6},

                                                                                    {BITWISE_RSHIFT, 7},
                                                                                    {BITWISE_LSHIFT, 7},

                                                                                    {LESS, 9},
                                                                                    {LESS_EQ, 9},
                                                                                    {GREATER, 9},
                                                                                    {GREATER_EQ, 9},

                                                                                    {EQUAL, 10},
                                                                                    {NOT_EQUAL, 10},

                                                                                    {BITWISE_AND, 11},
                                                                                    {BITWISE_XOR, 12},
                                                                                    {BITWISE_OR, 13},

                                                                                    {ASSIGN, 16},
                                                                                    {ADD_ASSIGN, 16},
                                                                                    {SUBTRACT_ASSIGN, 16},
                                                                                    {MULT_ASSIGN, 16},
                                                                                    {DIV_ASSIGN, 16},
                                                                                    {MOD_ASSIGN, 16},
                                                                                    {BITWISE_LSHIFT_ASSIGN, 16},
                                                                                    {BITWISE_RSHIFT_ASSIGN, 16},
                                                                                    {BITWISE_AND_ASSIGN, 16},
                                                                                    {BITWISE_OR_ASSIGN, 16},
                                                                                    {BITWISE_XOR_ASSIGN, 16}};

    const inline std::unordered_map<std::string, operator_type> prefix_operators {
        {"+", POSITIVE}, {"-", NEGATIVE},    {"*", INDIRECTION}, {"&", ADDRESSOF},
        {"!", NOT},      {"~", BITWISE_NOT}, {"++", INCREMENT},  {"--", DECREMENT}};

    const inline std::unordered_map<std::string, operator_type> postfix_operators {
        {"++", INCREMENT},
        {"--", DECREMENT},
    };
}

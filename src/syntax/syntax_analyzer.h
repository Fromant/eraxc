//
// Created by frama on 22.10.2024.
//

#ifndef BLCK_COMPILER_SYNTAX_ANALYZER_H
#define BLCK_COMPILER_SYNTAX_ANALYZER_H


#include <unordered_map>
#include <string>

namespace blck::syntax {
    enum Keyword {
        i8, i16, i32, i64, i128, i256,
        u8, u16, u32, u64, u128, u256,
        INT, LONG, CHAR, BOOL, SHORT,
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

    extern std::unordered_map<std::string, Keyword> keywords;

    class syntax_analyzer {

    };

}

#endif //BLCK_COMPILER_SYNTAX_ANALYZER_H

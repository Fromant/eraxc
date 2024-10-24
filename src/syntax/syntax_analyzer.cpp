//
// Created by frama on 22.10.2024.
//

#include "syntax_analyzer.h"


namespace blck::syntax {
    std::unordered_map<std::string, Keyword> keywords{
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
}

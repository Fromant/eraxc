#pragma once

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

}
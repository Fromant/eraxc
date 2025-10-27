#pragma once

#include "../JIR/CFG/CFG.h"
namespace eraxc {

    enum ARCH {
        X64,
        ARM
    };

    template<ARCH arch>
    struct asm_translator {
        virtual void translate(const JIR::CFG& JIR, const std::string& o_filename) = 0;
        virtual ~asm_translator() = default;
    };


}

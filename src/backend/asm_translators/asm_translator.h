//
// Created by frama on 01.02.2025.
//

#ifndef BLCK_COMPILER_ASM_TRANSLATOR_H
#define BLCK_COMPILER_ASM_TRANSLATOR_H

#include "../JIR_handler.h"

namespace eraxc {

    enum ARCH {
        X64, ARM
    };

    template<ARCH arch>
    struct asm_translator {
        virtual void translate(const JIR::JIR_handler &JIR, const std::string &o_filename) = 0;
        virtual ~asm_translator() = default;
    };


}


#endif //BLCK_COMPILER_ASM_TRANSLATOR_H

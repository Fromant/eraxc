#ifndef BLCK_COMPILER_ASM_TRANSLATOR_H
#define BLCK_COMPILER_ASM_TRANSLATOR_H

#include "../frontend/syntax/syntax_analyzer.h"

namespace blck::backend {

    struct asm_translator {

        struct data_label {
            std::string label;
            size_t init_val;
        };

        static std::string translate_NASM(const std::vector<syntax::AST::node>&, const std::string& o_filename);



    };

}

#endif //BLCK_COMPILER_ASM_TRANSLATOR_H

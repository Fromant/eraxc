#ifndef ERAXC_ASM_TRANSLATOR_H
#define ERAXC_ASM_TRANSLATOR_H

#include "../legacy/syntax_analyzer.h"

namespace eraxc::backend {

    struct asm_translator {

        static std::string translate_NASM(const std::vector<syntax::AST::node> &, const std::string &o_filename);


    };

}

#endif //ERAXC_ASM_TRANSLATOR_H

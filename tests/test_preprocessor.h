#ifndef ERAXC_TEST_PREPROCESSOR_H
#define ERAXC_TEST_PREPROCESSOR_H

#include "../src/frontend/lexic/preprocessor_tokenizer.h"

namespace tests {
    int test_preprocessor_tokenizer() {

        //test 1 - #define macro, #ifdef #ifndef
        eraxc::tokenizer t1{};
        auto r1 = t1.tokenize_file("../tests/files/preprocessor_define.erx");
        std::vector<eraxc::token> res1{{eraxc::token::INSTANT, "123"},{eraxc::token::IDENTIFIER, "test11"}};
        assert(r1);
        assert(r1.value == res1);

        return 0;
    }
}


#endif //ERAXC_TEST_PREPROCESSOR_H

#ifndef BLCK_COMPILER_TEST_PREPROCESSOR_H
#define BLCK_COMPILER_TEST_PREPROCESSOR_H

#include "../src/frontend/lexic/preprocessor.h"

namespace tests {
    int test_preprocessor() {

        //test 1 - #define macro, #ifdef #ifndef
        assert(blck::preprocess("../tests/files/preprocessor_define.erx") == "\n123 \n\n\ntest11 \n\n");

        //test 2 - #include macro
        assert(blck::preprocess("../tests/files/preprocessor_include.erx") == "\n123 \n\n\ntest11 \n\n\n");

        return 0;
    }
}


#endif //BLCK_COMPILER_TEST_PREPROCESSOR_H

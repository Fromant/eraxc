#ifndef ERAXC_TEST_PREPROCESSOR_H
#define ERAXC_TEST_PREPROCESSOR_H

#include "../src/frontend/lexic/preprocessor.h"

namespace tests {
    int test_preprocessor() {

        //test 1 - #define macro, #ifdef #ifndef
        assert(eraxc::preprocess("../tests/files/preprocessor_define.erx") == "\n123 \n\n\ntest11 \n\n");

        //test 2 - #include macro
        assert(eraxc::preprocess("../tests/files/preprocessor_include.erx") == "\n123 \n\n\ntest11 \n\n\n");

        return 0;
    }
}


#endif //ERAXC_TEST_PREPROCESSOR_H

#ifndef ERAXC_TEST_ALL_H
#define ERAXC_TEST_ALL_H

#ifdef TEST

#include "test_preprocessor.h"

#endif
#include "JIR/test_jir.h"


bool test_all() {
    #ifdef TEST

    int preprocessor = tests::test_preprocessor_tokenizer();
    if (preprocessor != 0) {
        std::cout << "Preprocessor failed " << preprocessor << "/" << ALL_TESTS_PREPROCESSOR << " tests\n";
        return false;
    } else {
        std::cout << "Preprocessor passed all tests!\n";
    }

    int jir = tests::test_jir();
    if (jir != 0) {
        std::cout << "JIR failed " << jir << "/" << ALL_TESTS_JIR << " tests\n";
        return false;
    } else {
        std::cout << "JIR passed all tests!\n";
    }

    std::cerr.flush();
    std::cout.flush();

    #endif
    return true;
}

#endif //ERAXC_TEST_ALL_H

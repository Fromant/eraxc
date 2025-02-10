#ifndef ERAXC_TEST_ALL_H
#define ERAXC_TEST_ALL_H

#ifdef TEST

#include "test_ArrayTree.h"
#include "test_preprocessor.h"
#include "test_lexer.h"
#include "test_syntax_analyzer.h"

#endif


bool test_all() {
    #ifdef TEST

    auto f = tests::test_preprocessor_tokenizer();
    if (f != 0) {
        std::cout << "Preprocessor failed " << f << "/" << ALL_TESTS_PREPROCESSOR << " tests\n";
        return false;
    } else {
        std::cout << "Preprocessor passed all tests!\n";
    }

    #endif
    return true;
}

#endif //ERAXC_TEST_ALL_H

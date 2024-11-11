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

    if (tests::test_ArrayTree() != 0) {
        std::cout << "Test in util/ArrayTree.h is failed...\n";
        return false;
    } else {
        std::cout << "util/ArrayTree.h passed all tests!\n";
    }

    if (tests::test_preprocessor() != 0) {
        std::cout << "Test in preprocessor is failed...\n";
        return false;
    } else {
        std::cout << "Preprocessor passed all tests!\n";
    }

    if (tests::test_lexer() != 0) {
        std::cout << "Test in lexer is failed...\n";
        return false;
    } else {
        std::cout << "Lexer passed all tests!\n";
    }

    if (tests::test_syntax_analyzer() != 0) {
        std::cout << "Test in syntax_analyzer is failed...\n";
        return false;
    } else {
        std::cout << "Syntax_analyzer passed all tests!\n";
    }

    #endif
    return true;
}

#endif //ERAXC_TEST_ALL_H

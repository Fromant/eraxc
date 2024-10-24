//
// Created by frama on 23.10.2024.
//

#ifndef BLCK_COMPILER_TEST_ALL_H
#define BLCK_COMPILER_TEST_ALL_H

#ifdef TEST

#include "test_ArrayTree.h"

#endif


bool test_all() {
    #ifdef TEST

    if (tests::test_ArrayTree() != 0) {
        std::cout << "Test in util/ArrayTree.h is failed...\n";
        return false;
    } else {
        std::cout << "util/ArrayTree.h passed all tests!\n";
    }


    #endif
    return true;
}

#endif //BLCK_COMPILER_TEST_ALL_H

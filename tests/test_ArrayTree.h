#ifndef ERAXC_TEST_ARRAYTREE_H
#define ERAXC_TEST_ARRAYTREE_H

#include "../src/util/ArrayTree.h"
#include <cassert>

namespace tests {
    int test_ArrayTree() {

        eraxc::ArrayTree<int> test{};

        assert(test.root() == 0);
        assert(test.getLeft(0) == 0);
        test.getRight(0) = 1;
        assert(test.getRight(0) == 1);
        test.getLeft(1) = 12;
        assert(test.getLeft(1) == 12);
        assert(test.getRight(1) == 0);
        assert(test.getRight(0) == 1);
        assert(test.getLeft(156) == 0);


        //same with harder struct, e.g. std::string
        eraxc::ArrayTree<std::string> test1{};
        assert(test1.root().empty());
        assert(test1.getLeft(0).empty());
        test1.getRight(0) = "1";
        assert(test1.getRight(0) == "1");
        test1.getLeft(1) = "12";
        assert(test1.getLeft(1) == "12");
        assert(test1.getRight(1).empty());
        assert(test1.getRight(0) == "1");
        assert(test1.getLeft(156).empty());


        return 0;

    }
}


#endif //ERAXC_TEST_ARRAYTREE_H

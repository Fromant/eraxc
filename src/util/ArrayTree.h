#ifndef ERAXC_ARRAYTREE_H
#define ERAXC_ARRAYTREE_H

#include <vector>
#include <string>

namespace eraxc {

    template<typename T>
    struct ArrayTree {
        ArrayTree() {
            nodes = std::vector<T>(16);
        }

        explicit ArrayTree(int capacity) {
            nodes = std::vector<T>(capacity);
        }

        T &getLeft(int parent) {
            if (nodes.size() < 2 * parent + 3) {
                nodes.resize(2 * parent + 3); //init children
            }
            return nodes[2 * parent + 1];
        }

        T &getRight(int parent) {
            if (nodes.size() < 2 * parent + 3) {
                nodes.resize(2 * parent + 3); //init children
            }
            return nodes[2 * parent + 2];
        }

        T &root() {
            if (nodes.empty()) {
                nodes.resize(16);
            }
            return nodes[0];
        }

    private:
        std::vector<T> nodes;
    };

} // eraxc

#endif //ERAXC_ARRAYTREE_H

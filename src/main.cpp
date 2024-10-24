#include <iostream>
#include <chrono>

#include "lexic/preprocessor.h"
#include "lexic/lexic.h"


#define TEST

#include "../tests/test_all.h"

using namespace blck;


int main() {

    if (!test_all()) return -1;

    auto t1 = std::chrono::high_resolution_clock::now();

    auto s1 = preprocess("../examples/1.blck");
    auto s2 = preprocess("../examples/2.blck");
    auto s3 = preprocess("../examples/long-long.blck");

    auto t2 = std::chrono::high_resolution_clock::now();

    double dur = std::chrono::duration<double, std::milli>(t2 - t1).count();
    std::cout << s1;
    std::cout << "Preprocessor done in: " << dur << "ms\n";
    std::cout << "Speed: " << (double) (s3.size() + s1.size() + s2.size()) / 1024 / 1024 / dur * 1000 << "mb/s\n";


    t1 = std::chrono::high_resolution_clock::now();

    auto tokens = lexic::tokenize(s1);
    auto tokens2 = lexic::tokenize(s2);
    auto tokens3 = lexic::tokenize(s3);

    t2 = std::chrono::high_resolution_clock::now();
    dur = std::chrono::duration<double, std::milli>(t2 - t1).count();
    for (const auto &t: tokens) {
        std::cout << t.t << ": " << t.data << '\n';
    }
    std::cout << "Lexer done in: " << dur << "ms\n";
    std::cout << "Speed: " << (double) (s3.size() + s1.size() + s2.size()) / 1024 / 1024 / dur * 1000 << "mb/s\n";
    return 0;
}

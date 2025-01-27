#include <iostream>
#include <chrono>

#include "backend/IL_handler.h"

#include "frontend/lexic/preprocessor_tokenizer.h"

#ifdef DEBUG
//#define TEST
//#include "../tests/test_all.h"
#endif

using namespace eraxc;

int main() {
    //TODO fix tests
    #ifdef DEBUG
    std::cout << "DEBUGtest\n";
//    if (!test_all()) return -1;
    #endif

    const std::string f1{"../examples/1.erx"};
    const std::string f2{"../examples/2.erx"};
    const std::string f3{"../examples/long-long.erx"};

    auto t1 = std::chrono::high_resolution_clock::now();
    tokenizer tokenizer;
    auto r = tokenizer.tokenize_file("../examples/1.erx");
    auto t2 = std::chrono::high_resolution_clock::now();
    double dur = std::chrono::duration<double, std::milli>(t2 - t1).count();
    std::cout << "preprocessor_tokenizer done in: " << dur << "ms\n";
    if (!r) {
        std::cerr << "Failed to tokenize. Error:\n" << r.error << std::endl;
        exit(-1);
    }

    t1 = std::chrono::high_resolution_clock::now();
    IL::IL_handler a{};
    a.translate(r.value);
    t2 = std::chrono::high_resolution_clock::now();
    dur = std::chrono::duration<double, std::milli>(t2 - t1).count();
    std::cout << "IL Handler done in: " << dur << "ms\n";

    return 0;
}

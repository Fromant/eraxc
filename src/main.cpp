#include <iostream>
#include <chrono>

#include "frontend/lexic/preprocessor.h"
#include "frontend/lexic/lexic.h"
#include "frontend/syntax/syntax_analyzer.h"
#include "backend/asm_translator.h"
#include "backend/IL_handler.h"

#define TEST

#include "../tests/test_all.h"

using namespace eraxc;

int main() {

    if (!test_all()) return -1;

    const std::string f1{"../examples/1.erx"};
    const std::string f2{"../examples/2.erx"};
    const std::string f3{"../examples/long-long.erx"};

    auto t1 = std::chrono::high_resolution_clock::now();

    auto s1 = preprocess(f1);
//    auto s2 = preprocess(f2);
//    auto s3 = preprocess(f3);

    auto t2 = std::chrono::high_resolution_clock::now();

    double dur = std::chrono::duration<double, std::milli>(t2 - t1).count();
//    std::cout << s1;
    std::cout << "Preprocessor done in: " << dur << "ms\n";
//    std::cout << "Speed: " << (double) (s3.size() + s1.size() + s2.size()) / 1024 / 1024 / dur * 1000 << "mb/s\n";
    std::cout << "Speed: " << (double) (s1.size()) / 1024 / 1024 / dur * 1000 << "mb/s\n";


    t1 = std::chrono::high_resolution_clock::now();

    auto tokens = lexic::tokenize(s1, f1);

    t2 = std::chrono::high_resolution_clock::now();
    dur = std::chrono::duration<double, std::milli>(t2 - t1).count();
    std::cout << "Lexer done in: " << dur << "ms\n";
    std::cout << "Speed: " << (double) (s1.size()) / 1024 / 1024 / dur * 1000 << "mb/s\n";

    t1 = std::chrono::high_resolution_clock::now();

    syntax::syntax_analyzer analyzer{};

    auto st1 = analyzer.analyze(tokens);

    t2 = std::chrono::high_resolution_clock::now();
    dur = std::chrono::duration<double, std::milli>(t2 - t1).count();
    std::cout << "Syntax analyzer done in: " << dur << "ms\n";

    if (!st1) {
        std::cout << "ERROR: " << st1.error << '\n';
    }

    for (auto &t: st1.value) {
        t.print();
    }

    t1 = std::chrono::high_resolution_clock::now();
    eraxc::IL::IL_handler a{st1.value};
    t2 = std::chrono::high_resolution_clock::now();
    dur = std::chrono::duration<double, std::milli>(t2 - t1).count();
    std::cout << "IL Handler done in: " << dur << "ms\n";

    backend::asm_translator::translate_NASM(st1.value, "test.asm");

    syntax::syntax_analyzer analyzer2{};

    std::string cycle = preprocess("../examples/cycle.erx");
    auto cycle_lexed = lexic::tokenize(cycle, "cycle.erx");
    auto t3 = analyzer2.analyze(cycle_lexed);
    if (!t3) std::cout << "ERROR: " << t3.error << '\n';

    return 0;
}
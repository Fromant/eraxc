#include <iostream>
#include <chrono>

#include "backend/IL_handler.h"
#include "frontend/lexic/preprocessor_tokenizer.h"

#include "util/IL_utils.h"

#include "backend/asm_translators/asm_x86.h"

#ifdef DEBUG
//#define TEST
//#include "../tests/test_all.h"
#endif

using namespace eraxc;

int main(int argc, char *argv[]) {
    //TODO fix tests
    #ifdef DEBUG
    std::cout << "DEBUG build. Running tests...\n";
//    if (!test_all()) return -1;
    #endif

    double total_time = 0;

    const std::string f1{"../examples/1.erx"};
    const std::string f2{"../examples/2.erx"};
    const std::string f3{"../examples/long-long.erx"};

    auto t1 = std::chrono::high_resolution_clock::now();
    tokenizer tokenizer;
    auto r = tokenizer.tokenize_file("../examples/0.erx");
    auto t2 = std::chrono::high_resolution_clock::now();
    double dur = std::chrono::duration<double, std::milli>(t2 - t1).count();
    total_time += dur;
    std::cout << "preprocessor_tokenizer done in: " << dur << "ms\n";
    if (!r) {
        std::cerr << "Failed to tokenize. Error:\n" << r.error << std::endl;
        exit(-1);
    }

    t1 = std::chrono::high_resolution_clock::now();
    IL::IL_handler a{};
    auto IL_err = a.translate(r.value);
    t2 = std::chrono::high_resolution_clock::now();
    if (!IL_err) {
        //error encountered
        std::cerr << "Failed to translate code to IL. Error:\n" << IL_err.error << std::endl;
        exit(-1);
    }
    dur = std::chrono::duration<double, std::milli>(t2 - t1).count();
    total_time += dur;
    std::cout << "IL Handler done in: " << dur << "ms\n";

    t1 = std::chrono::high_resolution_clock::now();
    asm_translator<X64> asmt{};
    auto asmtr = asmt.translate(a, "test1231.asm");
    t2 = std::chrono::high_resolution_clock::now();
    if (!asmtr) {
        std::cerr << "Failed to translate IL code to asm. Error:\n" << asmtr.error << std::endl;
        exit(-1);
    }
    dur = std::chrono::duration<double, std::milli>(t2 - t1).count();
    total_time += dur;
    std::cout << "ASM translator done in: " << dur << "ms\n";

    std::cout << "\nCompilation completed successfully in " << total_time << "ms\n\nGlobal init:\n";

    eraxc::IL::print_IL_nodes(a.global_init);

    std::cout << "\n\nAll funcs:\n";
    eraxc::IL::print_IL_funcs(a.global_funcs);

    return 0;
}

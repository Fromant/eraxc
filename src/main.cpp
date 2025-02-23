#include <iostream>
#include <chrono>

#include "backend/JIR_handler.h"
#include "frontend/lexic/preprocessor_tokenizer.h"
#include "util/JIR_utils.h"

#include "backend/asm_translators/asm_x86.h"

#ifdef DEBUG
#define TEST
#include "../tests/test_all.h"
#endif

using namespace eraxc;

error::errable<void> compilation_pipeline(const std::string& filename) {
    double total_time = 0;

    auto t1 = std::chrono::high_resolution_clock::now();
    tokenizer tokenizer;
    auto r = tokenizer.tokenize_file(filename);
    auto t2 = std::chrono::high_resolution_clock::now();
    double dur = std::chrono::duration<double, std::milli>(t2 - t1).count();
    total_time += dur;
    std::cout << "preprocessor_tokenizer done in: " << dur << "ms\n";
    if (!r) {
        return {"Failed to tokenize file " + filename + ". Error:\n" + r.error};
    }

    t1 = std::chrono::high_resolution_clock::now();
    JIR::JIR_handler a{};
    auto JIR_err = a.translate(r.value);
    t2 = std::chrono::high_resolution_clock::now();
    if (!JIR_err) {
        return {"Failed to translate to IL code. Error:\n" + JIR_err.error};
    }
    dur = std::chrono::duration<double, std::milli>(t2 - t1).count();
    total_time += dur;
    std::cout << "IL Handler done in: " << dur << "ms\n";

    std::cout << "\n\nGlobal init:\n";
    print_JIR_nodes(a.global_variables_init);

    std::cout << "\n\nAll funcs:\n";
    print_JIR_funcs(a.global_funcs);

    t1 = std::chrono::high_resolution_clock::now();
    asm_translator<X64> asmt{};
    auto asmtr = asmt.translate(a, "eraxc.asm");
    t2 = std::chrono::high_resolution_clock::now();
    if (!asmtr) {
        return {"Failed to translate to IL code. Error:\n" + asmtr.error};
    }
    dur = std::chrono::duration<double, std::milli>(t2 - t1).count();
    total_time += dur;
    std::cout << "ASM translator done in: " << dur << "ms\n";

    std::cout << "\nCompilation completed successfully in " << total_time << "ms\n";

    //autorun compilation to .exe
    system("nasm -f win64 eraxc.asm -o eraxc.obj");
    system("gcc eraxc.obj -o a.exe");

    return {""};
}

int main(int argc, char* argv[]) {
    #ifdef DEBUG
    std::cout << "DEBUG build. Running tests...\n";
    if (!test_all()) return -1;
    #endif

    auto err = compilation_pipeline("../examples/main.erx");
    if (!err) {
        std::cerr << err.error << std::endl;
        exit(-1);
    }

    return 0;
}

#include <iostream>
#include <chrono>

#include "backend/JIR/CFG/CFG.h"
#include "frontend/lexic/preprocessor_tokenizer.h"
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
    auto tokens = tokenizer.tokenize_file(filename);
    auto t2 = std::chrono::high_resolution_clock::now();
    double dur = std::chrono::duration<double, std::milli>(t2 - t1).count();
    total_time += dur;
    std::cout << "preprocessor_tokenizer done in: " << dur << "ms\n";
    if (!tokens) {
        return {"Failed to tokenize file " + filename + ". Error:\n" + tokens.error};
    }

    t1 = std::chrono::high_resolution_clock::now();
    JIR::CFG cfg{};
    auto JIR_err = cfg.create(tokens.value);
    t2 = std::chrono::high_resolution_clock::now();
    if (!JIR_err) {
        return {"Failed to translate to JIR code. Error:\n" + JIR_err.error};
    }
    dur = std::chrono::duration<double, std::milli>(t2 - t1).count();
    total_time += dur;
    std::cout << "CFG created in: " << dur << "ms\n";

    cfg.print_nodes();

    t1 = std::chrono::high_resolution_clock::now();
    asm_translator<X64> asmt{};
    auto asmtr = asmt.translate(cfg, "eraxc.asm");
    t2 = std::chrono::high_resolution_clock::now();
    if (!asmtr) {
        return {"Failed to translate to ASM. Error:\n" + asmtr.error};
    }
    dur = std::chrono::duration<double, std::milli>(t2 - t1).count();
    total_time += dur;
    std::cout << "ASM translator done in: " << dur << "ms\n";

    std::cout << "\nTranslation completed successfully in " << total_time << "ms\n";

    //autorun compilation to .exe
    t1 = std::chrono::high_resolution_clock::now();
    system("nasm -f win64 eraxc.asm -o eraxc.obj");
    t2 = std::chrono::high_resolution_clock::now();
    dur = std::chrono::duration<double, std::milli>(t2 - t1).count();
    std::cout << "nasm compiler done in: " << dur<<"ms\n";
    total_time += dur;

    t1 = std::chrono::high_resolution_clock::now();
    system("gcc eraxc.obj -o a.exe");
    t2 = std::chrono::high_resolution_clock::now();
    dur = std::chrono::duration<double, std::milli>(t2 - t1).count();
    std::cout << "gcc linker done in: " << dur<<"ms\n";
    total_time += dur;

    std::cout << "\nCompilation completed successfully in " << total_time << "ms\n";

    return {""};
}

int main(int argc, char* argv[]) {
    #ifdef DEBUG
    std::cout << "DEBUG build. Running tests...\n";
    if (!test_all()) return -1;
    #endif

    auto err = compilation_pipeline("../examples/0.erx");
    if (!err) {
        std::cerr << err.error << std::endl;
        exit(-1);
    }

    return 0;
}

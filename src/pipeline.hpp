#pragma once

#include "backend/allocation/StackAllocator.hpp"
#include "frontend/lexic/PreprocessorTokenizer.hpp"
#include "frontend/syntax/parser/StructureAnalyzer.hpp"

using namespace eraxc;

inline error::errable<void> compilation_pipeline(const std::string& filename) {
    double total_time = 0;

    auto t1 = std::chrono::high_resolution_clock::now();
    frontend::Tokenizer tokenizer;
    auto tokens = tokenizer.tokenize_file(filename);
    auto t2 = std::chrono::high_resolution_clock::now();
    double dur = std::chrono::duration<double, std::milli>(t2 - t1).count();
    total_time += dur;
    std::cout << "preprocessor_tokenizer done in: " << dur << "ms\n";
    if (!tokens) {
        return {"Failed to tokenize file " + filename + ". Error:\n" + tokens.error};
    }

    t1 = std::chrono::high_resolution_clock::now();
    frontend::StructureAnalyzer structureAnalyzer;
    const auto structure = structureAnalyzer.analyze(tokens.value);
    t2 = std::chrono::high_resolution_clock::now();
    if (!structure) {
        return {"Failed to translate to JIR code. Error:\n" + structure.error};
    }
    dur = std::chrono::duration<double, std::milli>(t2 - t1).count();
    total_time += dur;
    std::cout << "CFG created in: " << dur << "ms\n";

    structure.value.print();

    structure.value.print_to_file("cfg.txt");

    // cfg.print_to_file("cfg.txt");

    t1 = std::chrono::high_resolution_clock::now();

    for (const auto& function : structure.value.functions) {
        allocation::StackAllocator alloc {structure.value.globals};
        const auto allocatedCFG = alloc.allocate(function);
        if (!allocatedCFG) {
            return allocatedCFG.error;
        }
    }

    t2 = std::chrono::high_resolution_clock::now();
    dur = std::chrono::duration<double, std::milli>(t2 - t1).count();
    std::cout << "Allocation pass in: " << dur << "ms\n";


    t1 = std::chrono::high_resolution_clock::now();
    // auto asmtr = x64::asm_translator::translate(cfg_allocated, "eraxc.asm");
    t2 = std::chrono::high_resolution_clock::now();
    // if (!asmtr) {
    // return {"Failed to translate to ASM. Error:\n" + asmtr.error};
    // }
    dur = std::chrono::duration<double, std::milli>(t2 - t1).count();
    total_time += dur;
    std::cout << "ASM translator done in: " << dur << "ms\n";

    std::cout << "\nTranslation completed successfully in " << total_time << "ms\n";

    //autorun compilation to .exe
    t1 = std::chrono::high_resolution_clock::now();
    // system("nasm -f win64 eraxc.asm -o eraxc.obj");
    // system("D:/programs/SASM/Windows/NASM/nasm.exe -f win64 eraxc.asm -o eraxc.obj");
    t2 = std::chrono::high_resolution_clock::now();
    dur = std::chrono::duration<double, std::milli>(t2 - t1).count();
    std::cout << "nasm compiler done in: " << dur << "ms\n";
    total_time += dur;

    t1 = std::chrono::high_resolution_clock::now();
    // system("D:/programs/SASM/Windows/MinGW64/bin/gcc.exe eraxc.obj -o a.exe -m64 -g");
    // system("gcc eraxc.obj -o a.exe -m64 -g");
    t2 = std::chrono::high_resolution_clock::now();
    dur = std::chrono::duration<double, std::milli>(t2 - t1).count();
    std::cout << "gcc linker done in: " << dur << "ms\n";
    total_time += dur;

    std::cout << "\nCompilation completed successfully in " << total_time << "ms\n";


    return {""};
}
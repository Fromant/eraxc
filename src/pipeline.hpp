#pragma once

#include "backend/allocation/StackAllocator.hpp"
#include "backend/codegen/x64/asm_x64.hpp"
#include "common/JIR/Program.hpp"
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

    JIR::Program program {structure.value.functions, structure.value.globals, structure.value.entrypointId};

    t1 = std::chrono::high_resolution_clock::now();

    allocation::StackAllocator allocator;

    const auto allocatedProgram = allocator.allocate(program);

    t2 = std::chrono::high_resolution_clock::now();

    if (!allocatedProgram) {
        return {"Failed to allocate program. Error:\n" + structure.error};
    }

    dur = std::chrono::duration<double, std::milli>(t2 - t1).count();
    std::cout << "Allocation pass in: " << dur << "ms\n";

    int code = 0;


    t1 = std::chrono::high_resolution_clock::now();
    auto translationResult = x64::asm_translator::translate(allocatedProgram.value, "eraxc.asm");
    t2 = std::chrono::high_resolution_clock::now();
    if (!translationResult) {
        return {"Failed to translate to ASM. Error:\n" + translationResult.error};
    }
    dur = std::chrono::duration<double, std::milli>(t2 - t1).count();
    total_time += dur;
    std::cout << "ASM translator done in: " << dur << "ms\n";

    std::cout << "\nTranslation completed successfully in " << total_time << "ms\n";

    //autorun compilation to .exe
    t1 = std::chrono::high_resolution_clock::now();
    // system("nasm -f win64 eraxc.asm -o eraxc.obj");
    code = std::system("D:/programs/SASM/Windows/NASM/nasm.exe -f win64 eraxc.asm -o eraxc.obj");
    if (code != 0) {
        return {"nasm compiler failed with error"};
    }
    t2 = std::chrono::high_resolution_clock::now();
    dur = std::chrono::duration<double, std::milli>(t2 - t1).count();
    std::cout << "nasm compiler done in: " << dur << "ms\n";
    total_time += dur;

    t1 = std::chrono::high_resolution_clock::now();
    code = std::system("D:/programs/SASM/Windows/MinGW64/bin/gcc.exe eraxc.obj -o a.exe -m64 -g");
    if (code != 0) {
        return {"gcc linker failed with error"};
    }
    // system("gcc eraxc.obj -o a.exe -m64 -g");
    t2 = std::chrono::high_resolution_clock::now();
    dur = std::chrono::duration<double, std::milli>(t2 - t1).count();
    std::cout << "gcc linker done in: " << dur << "ms\n";
    total_time += dur;

    std::cout << "\nCompilation completed successfully in " << total_time << "ms\n";


    return {""};
}
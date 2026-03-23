cmake_minimum_required(VERSION 3.14)
project(eraxCore)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -g")

add_library(eraxCore
        util/error.cpp
        backend/JIR/CFG/CFG.cpp
        frontend/syntax/scope/ScopeManager.cpp
        frontend/lexic/PreprocessorTokenizer.cpp
        backend/JIR/CFG/Allocated/AllocationManager.cpp
        backend/JIR/CFG/Allocated/Iterator/CFGIterator.cpp
        backend/codegen/x64/asm_x64.cpp
        frontend/syntax/parser/StructureAnalyzer.cpp
        frontend/syntax/parser/ExpressionParser.cpp
        common/CFG/CFGIterator.cpp
        common/CFG/CFGParts.cpp
        common/JIR/Command.cpp
        common/JIR/FrontendResult.cpp
)
target_include_directories(eraxCore PRIVATE ../src)



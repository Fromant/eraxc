cmake_minimum_required(VERSION 3.14)
project(eraxCore)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -g")

add_library(eraxCore
        util/error.cpp
        backend/codegen/asm_x86_mem.h
        backend/JIR/CFG/CFG.cpp
        backend/JIR/ScopeManager.cpp
        backend/JIR/ScopeManager.h
        frontend/lexic/preprocessor_tokenizer.cpp
)
target_include_directories(eraxCore PRIVATE ../src)



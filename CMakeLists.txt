cmake_minimum_required(VERSION 3.28)
project(eraxc)

set(CMAKE_CXX_STANDARD 20)

add_executable(eraxc src/main.cpp
        src/frontend/lexic/preprocessor.h
        src/frontend/lexic/lexic.cpp
        src/blck_compiler.cpp
        src/frontend/lexic/prepocessor.cpp
        src/frontend/syntax/syntax_analyzer.cpp
        src/frontend/syntax/enums.cpp
        src/util/ArrayTree.cpp
        tests/test_all.h
        src/backend/asm_translator.cpp
        src/backend/asm_translator.h
        src/util/error.cpp

)

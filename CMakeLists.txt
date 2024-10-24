cmake_minimum_required(VERSION 3.28)
project(blck-compiler)

set(CMAKE_CXX_STANDARD 20)

add_executable(blck-compiler src/main.cpp
        src/lexic/preprocessor.h
        src/lexic/lexic.cpp
        src/lexic/lexic.h
        src/blck_compiler.cpp
        src/blck_compiler.h
        src/lexic/prepocessor.cpp
        src/syntax/syntax_analyzer.cpp
        src/syntax/syntax_analyzer.h
        src/util/ArrayTree.cpp
        src/util/ArrayTree.h
        tests/test_all.h
)

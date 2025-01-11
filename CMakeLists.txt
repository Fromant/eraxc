cmake_minimum_required(VERSION 3.28)
project(eraxc)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG")

add_executable(eraxc src/main.cpp
        src/frontend/lexic/preprocessor.h
        src/frontend/lexic/lexic.cpp
        src/frontend/lexic/prepocessor.cpp
        src/frontend/syntax/syntax_analyzer.cpp
        src/frontend/syntax/enums.cpp
        src/util/ArrayTree.cpp
        tests/test_all.h
        src/backend/asm_translator.cpp
        src/util/error.cpp
        src/backend/IL_handler.cpp
        src/frontend/lexic/preprocessor_tokenizer.cpp
        src/frontend/lexic/preprocessor_tokenizer.h
        src/util/triplet.h
)

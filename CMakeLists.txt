cmake_minimum_required(VERSION 3.28)
project(eraxc)

set(CMAKE_CXX_STANDARD 20)

add_executable(eraxc src/main.cpp
        src/frontend/syntax/enums.cpp
        src/util/ArrayTree.cpp
        src/backend/asm_translator.cpp
        src/util/error.cpp
        src/backend/IL_handler.cpp
)

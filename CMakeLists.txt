cmake_minimum_required(VERSION 3.28)
project(eraxc)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -g")

add_executable(eraxc src/main.cpp
        src/util/ArrayTree.cpp
        src/backend/asm_translator.cpp
        src/util/error.cpp
        src/backend/IL_handler.cpp
)

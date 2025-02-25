cmake_minimum_required(VERSION 3.28)
project(eraxc)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -g")

add_executable(eraxc src/main.cpp
        src/util/error.cpp
        src/backend/JIR_handler.cpp
        src/backend/JIR_handler.h
        src/util/JIR_utils.h
        src/backend/asm_translators/asm_x86_mem.h
)

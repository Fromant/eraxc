cmake_minimum_required(VERSION 3.14)
project(eraxc)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -g")

add_executable(eraxc main.cpp)
add_subdirectory(src)

target_link_libraries(eraxc eraxCore)
target_include_directories(eraxc PRIVATE src/)

add_subdirectory(tests)


cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(eraxTests)

include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0
)

FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE TEST_SOURCES "*.cpp")
add_executable(eraxTests ${TEST_SOURCES})

target_include_directories(eraxTests PRIVATE ../src)

target_link_libraries(eraxTests PRIVATE GTest::gtest_main eraxCore)


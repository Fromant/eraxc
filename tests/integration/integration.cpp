#include <gtest/gtest.h>

#include <algorithm>
#include <string>

#include "pipeline.hpp"

int executeProgramSimple(const std::string& program_path) {
    return std::system((program_path).c_str());
}

void checkSource(const std::string& path, int expectedExitCode) {
    if (auto err = compilation_pipeline(path); !err) {
        FAIL() << err.error;
    }
    EXPECT_EQ(executeProgramSimple("a.exe"), expectedExitCode);
}

const std::string prefix = "../../tests/files/integration/";

struct TestData {
    TestData(const std::string& path, int expectedOutput) : path(prefix + path), expectedOutput(expectedOutput) {}
    std::string path;
    int expectedOutput;
};

class IntegrationTest : public testing::TestWithParam<TestData> {};

TEST_P(IntegrationTest, integrationTest) {
    const TestData& data = GetParam();
    checkSource(data.path, data.expectedOutput);
}

std::vector<TestData> tests = {
    // simple expressions
    {"expressions/self_assignment.erx", 14},

    // if statements
    {"if/simpleIf.erx", 1},
    {"if/if_else.erx", 14},
    {"if/if_diff_stack.erx", 5},
    {"if/if_true.erx", 1},
    {"if/if_false.erx", 0},
    {"if/if_no_braces.erx", 1},
    {"if/if_else_no_braces.erx", 2},
    {"if/if_empty_body.erx", 42},
    {"if/if_else_if.erx", 2},
    {"if/if_compound_cond.erx", 3},
    {"if/if_not_cond.erx", 1},
    {"if/if_var_in_block.erx", 5},
    {"if/if_var_in_block_else.erx", 10},
    {"if/if_else_return.erx", 20},
    {"if/if_return_else.erx", 1},
    {"if/if_all_comparisons.erx", 255},
    {"if/if_nested_deep.erx", 42},
    {"if/if_nested_mixed.erx", 20},
    {"if/if_with_function_call_cond.erx", 42},
    {"if/nestedIf.erx", 2},
    {"if/nested_if_on_else.erx", 2},
    {"if/nested_double_if_else.erx", 2},
    {"if/nested_if_with_else.erx", 2},
    {"if/return_inside_block.erx", 4},
    {"if/if_with_self_assignment.erx", 255},
    // function calls
    {"functions/call.erx", 2},
    {"functions/nested_call.erx", 2},
    {"functions/recursion.erx", 120},
    // global variables
    {"globals/globals_init.erx", 14},
    {"globals/globals_usage.erx", 136},
    {"globals/globals_shared.erx", 148},
    // loops
    {"loops/simpleWhile.erx", 17},
    // scope
    {"scope/scope.erx", 200},
    // misc
    {"misc/weird.erx", 102},
};

INSTANTIATE_TEST_SUITE_P(IntegrationTest, IntegrationTest, ::testing::ValuesIn(tests),
                         [](const testing::TestParamInfo<IntegrationTest::ParamType>& info) {
                             std::string full_string = info.param.path;
                             size_t last_slash_pos = full_string.find_last_of('/');

                             if (last_slash_pos != std::string::npos) {
                                 full_string = full_string.substr(last_slash_pos + 1);
                             }

                             full_string.erase(std::remove_if(full_string.begin(), full_string.end(),
                                                              [](unsigned char c) {
                                                                  return !std::isalnum(c);
                                                              }),
                                               full_string.end());

                             return full_string.substr(0, full_string.size() - 3);
                         });

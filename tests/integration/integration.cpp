#include <gtest/gtest.h>

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

class DynamicTest : public ::testing::TestWithParam<TestData> {};

TEST_P(DynamicTest, integrationTest) {
    const TestData& data = GetParam();
    checkSource(data.path, data.expectedOutput);
}

std::vector<TestData> tests = {
    {"call.erx", 2},
    {"if_diff_stack.erx", 5},
    {"if_else.erx", 14},
    {"nested_call.erx", 2},
    {"nestedIf.erx", 2},
    {"recursion.erx", 120},
    {"return_inside_block.erx", 4},
    {"scope.erx", 200},
    {"simpleIf.erx", 1},
    {"simpleWhile.erx", 17},
    {"weird.erx", 102},
};

class TestNameGenerator {
public:
    template<typename T>
    static std::string GetName(int i) {

        std::string full_string = tests[i].path;

        size_t last_slash_pos = full_string.find_last_of('/');

        if (last_slash_pos == std::string::npos) {
            return full_string;
        }

        return full_string.substr(last_slash_pos + 1);
    }
};

INSTANTIATE_TEST_SUITE_P(IntegrationTests, DynamicTest, ::testing::ValuesIn(tests),
                         [](const testing::TestParamInfo<DynamicTest::ParamType>& info) {
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

                             return full_string;
                         });

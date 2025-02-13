#ifndef ERAXC_TEST_PREPROCESSOR_H
#define ERAXC_TEST_PREPROCESSOR_H

#include "../src/frontend/lexic/preprocessor_tokenizer.h"

#define ALL_TESTS_PREPROCESSOR 4

namespace tests {
    inline int test_preprocessor_tokenizer() {
        std::cerr << "Running preprocessor tokenizer tests..." << std::endl;

        auto passed_tests = 0;

        //test 1 - #define macro, #ifdef #ifndef
        eraxc::tokenizer t1 {};
        auto r1 = t1.tokenize_file("../tests/files/preprocessor_define.erx");
        std::vector<eraxc::token> res1 {
            {eraxc::token::INSTANT, "123"}, {eraxc::token::IDENTIFIER, "anton"}, {eraxc::token::IDENTIFIER, "test11"}};

        if (!r1) {
            std::cerr << "Test 1 for preprocessor tokenizer failed with error: " << r1.error << std::endl;
        } else {
            if (r1.value != res1) {
                std::cerr << "Expected content: " << std::endl;
                for (const auto &i : res1) {
                    std::cerr << i.data << " ";
                }
                std::cerr << std::endl;
                std::cerr << "Got content: " << std::endl;
                for (const auto &i : r1.value) {
                    std::cerr << i.data << " ";
                }
                std::cerr << std::endl;
            } else {
                std::cerr << "Test 1 passed" << std::endl;
                passed_tests++;
            }
        }
        //test 2 - all operators
        std::string str = "< = > & | ^ % * / ~ + - != <= >= == *= /= += -= %= <<= >>= |= &= ^=";
        std::stringstream ss(str);
        auto r2 = t1.tokenize(ss);

        if (!r1) {
            std::cerr << "Test 2 for preprocessor tokenizer failed with error: " << r2.error << std::endl;
        } else {
            bool is_passed = true;
            for (const auto &i : r2.value) {
                if (i.t != eraxc::token::type::OPERATOR) {
                    std::cerr << "Test 2 failed: types dont match for \"" << i.data << "\" operator" << std::endl;
                    is_passed = false;
                    break;
                };
            }
            if (is_passed) {
                std::cerr << "Test 2 passed" << std::endl;
                passed_tests++;
            }
        }

        //test 3 - other special symbols
        std::string str2 = "; ! () {} [] ? : . ,";
        std::stringstream ss2(str2);
        auto r3 = t1.tokenize(ss2);

        std::vector<eraxc::token::type> res3 {eraxc::token::type::SEMICOLON,
            eraxc::token::type::OPERATOR,
            eraxc::token::type::L_BRACKET,
            eraxc::token::type::R_BRACKET,
            eraxc::token::type::L_F_BRACKET,
            eraxc::token::type::R_F_BRACKET,
            eraxc::token::type::L_SQ_BRACKET,
            eraxc::token::type::R_SQ_BRACKET,
            eraxc::token::type::OPERATOR,
            eraxc::token::type::COLON,
            eraxc::token::type::DOT,
            eraxc::token::type::COMMA};

        if (!r3) {
            std::cerr << "Test 3 for preprocessor tokenizer failed with error: " << r3.error << std::endl;
        } else {
            if (r3.value.size() != res3.size()) {
                std::cerr << "Test 3 failed: sizes dont match: expected size: " << res3.size()
                          << "; got size: " << r3.value.size() << std::endl;
            } else {
                bool is_passed = true;
                for (size_t i = 0; i < res3.size(); ++i) {
                    if (r3.value[i].t != res3[i]) {
                        std::cerr << "Test 3 failed: type mismatch at position " << i << " - expected: " << res3[i]
                                  << ", got: " << r3.value[i].t << std::endl;
                        is_passed = false;
                        break;
                    }
                }
                if (is_passed) {
                    std::cerr << "Test 3 passed" << std::endl;
                    passed_tests++;
                }
            }
        }

        //test 4 - general tokenizer test
        auto r4 = t1.tokenize_file("../tests/files/tokenizer.erx");
        std::vector<eraxc::token> res4 {
            {eraxc::token::IDENTIFIER, "unsigned"},
            {eraxc::token::IDENTIFIER, "int"},
            {eraxc::token::IDENTIFIER, "fib"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::IDENTIFIER, "int"},
            {eraxc::token::IDENTIFIER, "number"},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::L_F_BRACKET, "{"},
            {eraxc::token::IDENTIFIER, "if"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::IDENTIFIER, "number"},
            {eraxc::token::OPERATOR, "<="},
            {eraxc::token::INSTANT, "0"},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::L_F_BRACKET, "{"},
            {eraxc::token::IDENTIFIER, "fprintf"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::IDENTIFIER, "stderr"},
            {eraxc::token::COMMA, ","},
            {eraxc::token::STRING_INSTANT, "Illegal Argument Is Passed!\\n"},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::SEMICOLON, ";"},
            {eraxc::token::IDENTIFIER, "exit"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::IDENTIFIER, "EXIT_FAILURE"},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::SEMICOLON, ";"},
            {eraxc::token::R_F_BRACKET, "}"},
            {eraxc::token::IDENTIFIER, "if"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::IDENTIFIER, "number"},
            {eraxc::token::OPERATOR, "=="},
            {eraxc::token::INSTANT, "1"},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::IDENTIFIER, "return"},
            {eraxc::token::INSTANT, "0"},
            {eraxc::token::SEMICOLON, ";"},
            {eraxc::token::IDENTIFIER, "if"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::IDENTIFIER, "number"},
            {eraxc::token::OPERATOR, "=="},
            {eraxc::token::INSTANT, "2"},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::IDENTIFIER, "return"},
            {eraxc::token::INSTANT, "1"},
            {eraxc::token::SEMICOLON, ";"},
            {eraxc::token::IDENTIFIER, "return"},
            {eraxc::token::IDENTIFIER, "fib"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::IDENTIFIER, "number"},
            {eraxc::token::OPERATOR, "-"},
            {eraxc::token::INSTANT, "1"},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::OPERATOR, "+"},
            {eraxc::token::IDENTIFIER, "fib"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::IDENTIFIER, "number"},
            {eraxc::token::OPERATOR, "-"},
            {eraxc::token::INSTANT, "2"},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::SEMICOLON, ";"},
            {eraxc::token::R_F_BRACKET, "}"},

            {eraxc::token::IDENTIFIER, "int"},
            {eraxc::token::IDENTIFIER, "getInput"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::IDENTIFIER, "void"},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::L_F_BRACKET, "{"},
            {eraxc::token::IDENTIFIER, "int"},
            {eraxc::token::IDENTIFIER, "num"},
            {eraxc::token::COMMA, ","},
            {eraxc::token::IDENTIFIER, "excess_len"},
            {eraxc::token::SEMICOLON, ";"},
            {eraxc::token::IDENTIFIER, "char"},
            {eraxc::token::IDENTIFIER, "buffer"},
            {eraxc::token::L_SQ_BRACKET, "["},
            {eraxc::token::INSTANT, "3"},
            {eraxc::token::R_SQ_BRACKET, "]"},
            {eraxc::token::COMMA, ","},
            {eraxc::token::OPERATOR, "*"},
            {eraxc::token::IDENTIFIER, "endPtr"},
            {eraxc::token::SEMICOLON, ";"},
            {eraxc::token::IDENTIFIER, "while"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::INSTANT, "1"},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::L_F_BRACKET, "{"},
            {eraxc::token::IDENTIFIER, "printf"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::STRING_INSTANT, "Please enter a valid number:"},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::SEMICOLON, ";"},
            {eraxc::token::IDENTIFIER, "fgets"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::IDENTIFIER, "buffer"},
            {eraxc::token::COMMA, ","},
            {eraxc::token::INSTANT, "3"},
            {eraxc::token::COMMA, ","},
            {eraxc::token::IDENTIFIER, "stdin"},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::SEMICOLON, ";"},
            {eraxc::token::IDENTIFIER, "excess_len"},
            {eraxc::token::OPERATOR, "="},
            {eraxc::token::INSTANT, "0"},
            {eraxc::token::SEMICOLON, ";"},
            {eraxc::token::IDENTIFIER, "if"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::OPERATOR, "!"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::IDENTIFIER, "buffer"},
            {eraxc::token::L_SQ_BRACKET, "["},
            {eraxc::token::INSTANT, "0"},
            {eraxc::token::R_SQ_BRACKET, "]"},
            {eraxc::token::OPERATOR, "=="},
            {eraxc::token::STRING_INSTANT, "\\n"},
            {eraxc::token::OPERATOR, "||"},
            {eraxc::token::IDENTIFIER, "buffer"},
            {eraxc::token::L_SQ_BRACKET, "["},
            {eraxc::token::INSTANT, "1"},
            {eraxc::token::R_SQ_BRACKET, "]"},
            {eraxc::token::OPERATOR, "=="},
            {eraxc::token::STRING_INSTANT, "\\n"},
            {eraxc::token::OPERATOR, "||"},
            {eraxc::token::IDENTIFIER, "buffer"},
            {eraxc::token::L_SQ_BRACKET, "["},
            {eraxc::token::INSTANT, "2"},
            {eraxc::token::R_SQ_BRACKET, "]"},
            {eraxc::token::OPERATOR, "=="},
            {eraxc::token::STRING_INSTANT, "\\n"},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::L_F_BRACKET, "{"},
            {eraxc::token::IDENTIFIER, "while"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::IDENTIFIER, "getchar"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::OPERATOR, "!="},
            {eraxc::token::STRING_INSTANT, "\\n"},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::IDENTIFIER, "excess_len"},
            {eraxc::token::OPERATOR, "++"},
            {eraxc::token::SEMICOLON, ";"},
            {eraxc::token::R_F_BRACKET, "}"},
            {eraxc::token::IDENTIFIER, "num"},
            {eraxc::token::OPERATOR, "="},
            {eraxc::token::IDENTIFIER, "strtol"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::IDENTIFIER, "buffer"},
            {eraxc::token::COMMA, ","},
            {eraxc::token::OPERATOR, "&"},
            {eraxc::token::IDENTIFIER, "endPtr"},
            {eraxc::token::COMMA, ","},
            {eraxc::token::INSTANT, "10"},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::SEMICOLON, ";"},
            {eraxc::token::IDENTIFIER, "if"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::IDENTIFIER, "excess_len"},
            {eraxc::token::OPERATOR, ">"},
            {eraxc::token::INSTANT, "0"},
            {eraxc::token::OPERATOR, "||"},
            {eraxc::token::IDENTIFIER, "num"},
            {eraxc::token::OPERATOR, ">"},
            {eraxc::token::INSTANT, "48"},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::OPERATOR, "||"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::OPERATOR, "*"},
            {eraxc::token::IDENTIFIER, "endPtr"},
            {eraxc::token::OPERATOR, "!="},
            {eraxc::token::STRING_INSTANT, "\\0"},
            {eraxc::token::OPERATOR, "&&"},
            {eraxc::token::OPERATOR, "*"},
            {eraxc::token::IDENTIFIER, "endPtr"},
            {eraxc::token::OPERATOR, "!="},
            {eraxc::token::STRING_INSTANT, "\\n"},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::OPERATOR, "||"},
            {eraxc::token::IDENTIFIER, "endPtr"},
            {eraxc::token::OPERATOR, "=="},
            {eraxc::token::IDENTIFIER, "buffer"},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::L_F_BRACKET, "{"},
            {eraxc::token::IDENTIFIER, "continue"},
            {eraxc::token::SEMICOLON, ";"},
            {eraxc::token::R_F_BRACKET, "}"},
            {eraxc::token::IDENTIFIER, "break"},
            {eraxc::token::SEMICOLON, ";"},
            {eraxc::token::R_F_BRACKET, "}"},
            {eraxc::token::IDENTIFIER, "printf"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::STRING_INSTANT, "\\nEntered digit: %d (it might take sometime)\\n"},
            {eraxc::token::COMMA, ","},
            {eraxc::token::IDENTIFIER, "num"},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::SEMICOLON, ";"},
            {eraxc::token::IDENTIFIER, "return"},
            {eraxc::token::IDENTIFIER, "num"},
            {eraxc::token::SEMICOLON, ";"},
            {eraxc::token::R_F_BRACKET, "}"},

            {eraxc::token::IDENTIFIER, "static"},
            {eraxc::token::IDENTIFIER, "void"},
            {eraxc::token::IDENTIFIER, "test"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::L_F_BRACKET, "{"},
            {eraxc::token::IDENTIFIER, "assert"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::IDENTIFIER, "fib"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::INSTANT, "5"},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::OPERATOR, "=="},
            {eraxc::token::INSTANT, "3"},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::SEMICOLON, ";"},
            {eraxc::token::IDENTIFIER, "assert"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::IDENTIFIER, "fib"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::INSTANT, "2"},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::OPERATOR, "=="},
            {eraxc::token::INSTANT, "1"},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::SEMICOLON, ";"},
            {eraxc::token::IDENTIFIER, "assert"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::IDENTIFIER, "fib"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::INSTANT, "9"},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::OPERATOR, "=="},
            {eraxc::token::INSTANT, "21"},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::SEMICOLON, ";"},
            {eraxc::token::R_F_BRACKET, "}"},

            {eraxc::token::IDENTIFIER, "int"},
            {eraxc::token::IDENTIFIER, "main"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::L_F_BRACKET, "{"},
            {eraxc::token::IDENTIFIER, "test"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::SEMICOLON, ";"},
            {eraxc::token::IDENTIFIER, "printf"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::STRING_INSTANT, "Tests passed...\\n"},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::SEMICOLON, ";"},
            {eraxc::token::IDENTIFIER, "printf"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::STRING_INSTANT, "Enter n to find nth fibonacci element...\\n"},
            {eraxc::token::STRING_INSTANT, "Note: You would be asked to enter input until valid number ( less "},
            {eraxc::token::STRING_INSTANT, "than or equal to 48 ) is entered.\\n"},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::SEMICOLON, ";"},
            {eraxc::token::IDENTIFIER, "int"},
            {eraxc::token::IDENTIFIER, "number"},
            {eraxc::token::OPERATOR, "="},
            {eraxc::token::IDENTIFIER, "getInput"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::SEMICOLON, ";"},
            {eraxc::token::IDENTIFIER, "clock_t"},
            {eraxc::token::IDENTIFIER, "start"},
            {eraxc::token::COMMA, ","},
            {eraxc::token::IDENTIFIER, "end"},
            {eraxc::token::SEMICOLON, ";"},
            {eraxc::token::IDENTIFIER, "start"},
            {eraxc::token::OPERATOR, "="},
            {eraxc::token::IDENTIFIER, "clock"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::SEMICOLON, ";"},
            {eraxc::token::IDENTIFIER, "printf"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::STRING_INSTANT, "Fibonacci element %d is %u "},
            {eraxc::token::COMMA, ","},
            {eraxc::token::IDENTIFIER, "number"},
            {eraxc::token::COMMA, ","},
            {eraxc::token::IDENTIFIER, "fib"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::IDENTIFIER, "number"},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::SEMICOLON, ";"},
            {eraxc::token::IDENTIFIER, "end"},
            {eraxc::token::OPERATOR, "="},
            {eraxc::token::IDENTIFIER, "clock"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::SEMICOLON, ";"},
            {eraxc::token::IDENTIFIER, "printf"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::STRING_INSTANT, "in %.3f seconds.\\n"},
            {eraxc::token::COMMA, ","},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::IDENTIFIER, "double"},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::L_BRACKET, "("},
            {eraxc::token::IDENTIFIER, "end"},
            {eraxc::token::OPERATOR, "-"},
            {eraxc::token::IDENTIFIER, "start"},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::OPERATOR, "/"},
            {eraxc::token::IDENTIFIER, "CLOCKS_PER_SEC"},
            {eraxc::token::R_BRACKET, ")"},
            {eraxc::token::SEMICOLON, ";"},
            {eraxc::token::IDENTIFIER, "return"},
            {eraxc::token::INSTANT, "0"},
            {eraxc::token::SEMICOLON, ";"},
            {eraxc::token::R_F_BRACKET, "}"},
        };


        if (!r4) {
            std::cerr << "Test 4 for preprocessor tokenizer failed with error: " << r4.error << std::endl;
        } else {
            if (r4.value.size() != res4.size()) {
                std::cerr << "Test 4 failed: sizes dont match: expected size: " << res4.size()
                          << "; got size: " << r4.value.size() << std::endl;
            } else {
                bool is_passed = true;
                for (size_t i = 0; i < res4.size(); ++i) {
                    if (r4.value[i].data != res4[i].data) {
                        std::cerr << "Test 4 failed: data mismatch at position " << i << " - expected: " << res4[i].data
                                  << ", got: " << r4.value[i].data << std::endl;
                        is_passed = false;
                        break;
                    }
                    if (r4.value[i].t != res4[i].t) {
                        std::cerr << "Test 4 failed: type mismatch at position " << i << " - expected: " << res4[i].data
                                  << ", got: " << r4.value[i].data << std::endl;
                        is_passed = false;
                        break;
                    }
                }
                if (is_passed) {
                    std::cerr << "Test 4 passed" << std::endl;
                    passed_tests++;
                }
            }
        }

        return ALL_TESTS_PREPROCESSOR - passed_tests;
    }
}


#endif  //ERAXC_TEST_PREPROCESSOR_H

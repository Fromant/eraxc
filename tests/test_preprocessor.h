#ifndef ERAXC_TEST_PREPROCESSOR_H
#define ERAXC_TEST_PREPROCESSOR_H

#include "../src/frontend/lexic/preprocessor_tokenizer.h"

#define ALL_TESTS_PREPROCESSOR 3

namespace tests {
    inline int test_preprocessor_tokenizer() {
        std::cerr << "Running preprocessor tokenizer tests..." << std::endl;

        auto passed_tests = 0;

        //test 1 - #define macro, #ifdef #ifndef
        eraxc::tokenizer t1{};
        auto r1 = t1.tokenize_file("../tests/files/preprocessor_define.erx");
        std::vector<eraxc::token> res1{{eraxc::token::INSTANT, "123"}, {eraxc::token::IDENTIFIER, "anton"},{eraxc::token::IDENTIFIER, "test11"}};

        if (!r1) {
            std::cerr << "Test 1 for preprocessor tokenizer failed with error: " << r1.error << std::endl;
        } else {
            if (r1.value != res1) {
                std::cerr << "Expected content: " << std::endl;
                for (const auto& i : res1) {
                    std::cerr << i.data << " ";
                }
                std::cerr << std::endl;
                std::cerr << "Got content: " << std::endl;
                for (const auto& i : r1.value) {
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
            for (const auto& i : r2.value) {
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

        std::vector<eraxc::token::type> res3{
            eraxc::token::type::SEMICOLON,
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
            eraxc::token::type::COMMA
        };

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
                        std::cerr << "Test 3 failed: type mismatch at position " << i
                            << " - expected: " << res3[i]
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
            {eraxc::token::STRING_INSTANT, "Illegal Argument Is Passed!\n"},
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
            {eraxc::token::IDENTIFIER, "+"},
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
            {eraxc::token::IDENTIFIER, "unsigned"},
            {eraxc::token::IDENTIFIER, "unsigned"},
            {eraxc::token::IDENTIFIER, "unsigned"},
            {eraxc::token::IDENTIFIER, "unsigned"},
            {eraxc::token::IDENTIFIER, "unsigned"},
            {eraxc::token::IDENTIFIER, "unsigned"},
            {eraxc::token::IDENTIFIER, "unsigned"},
            {eraxc::token::IDENTIFIER, "unsigned"},
            {eraxc::token::IDENTIFIER, "unsigned"},
            {eraxc::token::IDENTIFIER, "unsigned"},
            {eraxc::token::IDENTIFIER, "unsigned"},
            {eraxc::token::IDENTIFIER, "unsigned"},
        };

        std::cout << r4.error << std::endl;
        for (auto &i : r4.value) {
            std::cout << i.data << std::endl;
        }

        return ALL_TESTS_PREPROCESSOR - passed_tests;
    }
}


#endif  //ERAXC_TEST_PREPROCESSOR_H


#include <gtest/gtest.h>

#include "frontend/lexic/preprocessor_tokenizer.h"

TEST(TokenizerTest, ParseNumbers) {
    eraxc::tokenizer tokenizer;
    auto r = tokenizer.tokenize("123");
    ASSERT_EQ(r.error, "");
    ASSERT_EQ(r.value.size(), 1);
    EXPECT_EQ(r.value[0].t, eraxc::token::INSTANT);
    EXPECT_EQ(r.value[0].data, "123");
}

TEST(TokenizerTest, ParseFloatNumbers) {
    eraxc::tokenizer tokenizer;
    auto r = tokenizer.tokenize("123.2");
    ASSERT_EQ(r.error, "");
    ASSERT_EQ(r.value.size(), 1);
    EXPECT_EQ(r.value[0].t, eraxc::token::INSTANT);
    EXPECT_EQ(r.value[0].data, "123.2");
}

TEST(TokenizerTest, ParseSingleCharOperators) {
    eraxc::tokenizer tokenizer;
    auto checkResult = [&tokenizer](const std::string& src, char op) {
        auto res = tokenizer.tokenize(src);
        ASSERT_EQ(res.error, "");
        ASSERT_EQ(res.value.size(), 1);
        EXPECT_EQ(res.value[0].data, std::string(1, op));
        EXPECT_EQ(res.value[0].t, eraxc::token::OPERATOR);
    };

    for (const auto& op : eraxc::token::operator_chars) {
        checkResult(std::string() + op, op);
    }
}

TEST(TokenizerTest, ParseMultiCharOperators) {
    eraxc::tokenizer tokenizer;
    auto checkResult = [&tokenizer](const std::string& src, const std::string& op) {
        auto res = tokenizer.tokenize(src);
        ASSERT_EQ(res.error, "");
        ASSERT_EQ(res.value.size(), 1);
        EXPECT_EQ(res.value[0].data, op);
        EXPECT_EQ(res.value[0].t, eraxc::token::OPERATOR);
    };

    for (const auto& op : eraxc::token::operator_chars) {
        if (op == '/')
            continue;  //bypass because `///` is a comment
        checkResult(std::string(3, op), std::string(3, op));
    }
}

TEST(TokenizerTest, ParseAllOperators) {
    eraxc::tokenizer tokenizer;
    std::string str = "< = > & | ^ % * / ~ + - != <= >= == *= /= += -= %= <<= >>= |= &= ^=";
    auto res = tokenizer.tokenize(str);
    ASSERT_EQ(res.error, "");
    ASSERT_EQ(res.value.size(), 26);

    for (const auto& op : res.value) {
        EXPECT_EQ(op.t, eraxc::token::OPERATOR);
    }
}

TEST(TokenizerTest, ParseSpecialSymbols) {
    eraxc::tokenizer tokenizer;
    std::string str = "; ! () {} [] ? : . ,";

    const std::vector reference {
        eraxc::token::type::SEMICOLON,    eraxc::token::type::OPERATOR,     eraxc::token::type::L_BRACKET,
        eraxc::token::type::R_BRACKET,    eraxc::token::type::L_F_BRACKET,  eraxc::token::type::R_F_BRACKET,
        eraxc::token::type::L_SQ_BRACKET, eraxc::token::type::R_SQ_BRACKET, eraxc::token::type::OPERATOR,
        eraxc::token::type::COLON,        eraxc::token::type::DOT,          eraxc::token::type::COMMA};

    auto res = tokenizer.tokenize(str);
    ASSERT_EQ(res.error, "");
    ASSERT_EQ(res.value.size(), reference.size());

    for (auto i = 0; i < reference.size(); i++) {
        EXPECT_EQ(reference[i], res.value[i].t);
    }
}


TEST(TokenizerTest, GeneralTest) {
    eraxc::tokenizer tokenizer;
    std::string filepath = "../../tests/preprocessor/files/tokenizer.erx";

    const std::vector<eraxc::token> reference {
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

    auto compare_tokens = [](const eraxc::token& a, const eraxc::token& b) {
        if (a.t == b.t && a.data == b.data) {
            return testing::AssertionSuccess();
        }
        return testing::AssertionFailure() << a.data << " != " << b.data;
    };

    auto res = tokenizer.tokenize_file(filepath);
    ASSERT_EQ(res.error, "");
    ASSERT_EQ(res.value.size(), reference.size());

    for (auto i = 0; i < reference.size(); i++) {
        EXPECT_TRUE(compare_tokens(res.value[i], reference[i]));
    }
}


TEST(TokenizerTest, IgnoreComments) {
    eraxc::tokenizer tokenizer;
    auto r = tokenizer.tokenize("123.2 //comment till end of line//full line comment\n132.2");
    ASSERT_EQ(r.error, "");
    ASSERT_EQ(r.value.size(), 2);
    EXPECT_EQ(r.value[0].t, eraxc::token::INSTANT);
    EXPECT_EQ(r.value[0].data, "123.2");
    EXPECT_EQ(r.value[1].t, eraxc::token::INSTANT);
    EXPECT_EQ(r.value[1].data, "132.2");
}

TEST(TokenizerTest, IgnoreSpacesTabsLineBreaks) {
    eraxc::tokenizer tokenizer;
    auto r = tokenizer.tokenize("123.2  132.2 \t \t 1 12\t\t1");
    ASSERT_EQ(r.error, "");
    ASSERT_EQ(r.value.size(), 5);
    EXPECT_EQ(r.value[0].t, eraxc::token::INSTANT);
    EXPECT_EQ(r.value[0].data, "123.2");
    EXPECT_EQ(r.value[1].t, eraxc::token::INSTANT);
    EXPECT_EQ(r.value[1].data, "132.2");
    EXPECT_EQ(r.value[2].t, eraxc::token::INSTANT);
    EXPECT_EQ(r.value[2].data, "1");
    EXPECT_EQ(r.value[3].t, eraxc::token::INSTANT);
    EXPECT_EQ(r.value[3].data, "12");
    EXPECT_EQ(r.value[4].t, eraxc::token::INSTANT);
    EXPECT_EQ(r.value[4].data, "1");
}

TEST(TokenizerTest, Identifiers) {
    eraxc::tokenizer tokenizer;
    auto r = tokenizer.tokenize("test_123 test-123 _test 123test");
    ASSERT_EQ(r.error, "");
    ASSERT_EQ(r.value.size(), 5);
    EXPECT_EQ(r.value[0].t, eraxc::token::IDENTIFIER);
    EXPECT_EQ(r.value[0].data, "test_123");
    EXPECT_EQ(r.value[1].t, eraxc::token::IDENTIFIER);
    EXPECT_EQ(r.value[1].data, "test-123");
    EXPECT_EQ(r.value[2].t, eraxc::token::IDENTIFIER);
    EXPECT_EQ(r.value[2].data, "_test");
    EXPECT_EQ(r.value[3].t, eraxc::token::INSTANT);
    EXPECT_EQ(r.value[3].data, "123");
    EXPECT_EQ(r.value[4].t, eraxc::token::IDENTIFIER);
    EXPECT_EQ(r.value[4].data, "test");
}

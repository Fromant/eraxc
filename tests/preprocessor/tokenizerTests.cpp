
#include <gtest/gtest.h>

#include "frontend/lexic/PreprocessorTokenizer.hpp"

using namespace eraxc::frontend;

TEST(TokenizerTest, ParseNumbers) {
    Tokenizer tokenizer;
    auto r = tokenizer.tokenize("123");
    ASSERT_EQ(r.error, "");
    ASSERT_EQ(r.value.size(), 1);
    EXPECT_EQ(r.value[0].t, Token::INSTANT);
    EXPECT_EQ(r.value[0].data, "123");
}

TEST(TokenizerTest, ParseFloatNumbers) {
    Tokenizer tokenizer;
    auto r = tokenizer.tokenize("123.2");
    ASSERT_EQ(r.error, "");
    ASSERT_EQ(r.value.size(), 1);
    EXPECT_EQ(r.value[0].t, Token::INSTANT);
    EXPECT_EQ(r.value[0].data, "123.2");
}

TEST(TokenizerTest, ParseSingleCharOperators) {
    Tokenizer tokenizer;
    auto checkResult = [&tokenizer](const std::string& src, char op) {
        auto res = tokenizer.tokenize(src);
        ASSERT_EQ(res.error, "");
        ASSERT_EQ(res.value.size(), 1);
        EXPECT_EQ(res.value[0].data, std::string(1, op));
        EXPECT_EQ(res.value[0].t, Token::OPERATOR);
    };

    for (const auto& op : Token::operator_chars) {
        checkResult(std::string() + op, op);
    }
}

TEST(TokenizerTest, ParseMultiCharOperators) {
    Tokenizer tokenizer;
    auto checkResult = [&tokenizer](const std::string& src, const std::string& op) {
        auto res = tokenizer.tokenize(src);
        ASSERT_EQ(res.error, "");
        ASSERT_EQ(res.value.size(), 1);
        EXPECT_EQ(res.value[0].data, op);
        EXPECT_EQ(res.value[0].t, Token::OPERATOR);
    };

    for (const auto& op : Token::operator_chars) {
        if (op == '/')
            continue;  //bypass because `///` is a comment
        checkResult(std::string(3, op), std::string(3, op));
    }
}

TEST(TokenizerTest, ParseAllOperators) {
    Tokenizer tokenizer;
    std::string str = "< = > & | ^ % * / ~ + - != <= >= == *= /= += -= %= <<= >>= |= &= ^=";
    auto res = tokenizer.tokenize(str);
    ASSERT_EQ(res.error, "");
    ASSERT_EQ(res.value.size(), 26);

    for (const auto& op : res.value) {
        EXPECT_EQ(op.t, Token::OPERATOR);
    }
}

TEST(TokenizerTest, ParseSpecialSymbols) {
    Tokenizer tokenizer;
    std::string str = "; ! () {} [] ? : . ,";

    const std::vector reference {Token::type::SEMICOLON,    Token::type::OPERATOR,     Token::type::L_BRACKET,
                                 Token::type::R_BRACKET,    Token::type::L_F_BRACKET,  Token::type::R_F_BRACKET,
                                 Token::type::L_SQ_BRACKET, Token::type::R_SQ_BRACKET, Token::type::OPERATOR,
                                 Token::type::COLON,        Token::type::DOT,          Token::type::COMMA};

    auto res = tokenizer.tokenize(str);
    ASSERT_EQ(res.error, "");
    ASSERT_EQ(res.value.size(), reference.size());

    for (auto i = 0; i < reference.size(); i++) {
        EXPECT_EQ(reference[i], res.value[i].t);
    }
}


TEST(TokenizerTest, GeneralTest) {
    Tokenizer tokenizer;
    std::string filepath = "../../tests/files/preprocessor/tokenizer.erx";

    const std::vector<Token> reference {
        {Token::IDENTIFIER, "unsigned"},
        {Token::IDENTIFIER, "int"},
        {Token::IDENTIFIER, "fib"},
        {Token::L_BRACKET, "("},
        {Token::IDENTIFIER, "int"},
        {Token::IDENTIFIER, "number"},
        {Token::R_BRACKET, ")"},
        {Token::L_F_BRACKET, "{"},
        {Token::IDENTIFIER, "if"},
        {Token::L_BRACKET, "("},
        {Token::IDENTIFIER, "number"},
        {Token::OPERATOR, "<="},
        {Token::INSTANT, "0"},
        {Token::R_BRACKET, ")"},
        {Token::L_F_BRACKET, "{"},
        {Token::IDENTIFIER, "fprintf"},
        {Token::L_BRACKET, "("},
        {Token::IDENTIFIER, "stderr"},
        {Token::COMMA, ","},
        {Token::STRING_INSTANT, "Illegal Argument Is Passed!\\n"},
        {Token::R_BRACKET, ")"},
        {Token::SEMICOLON, ";"},
        {Token::IDENTIFIER, "exit"},
        {Token::L_BRACKET, "("},
        {Token::IDENTIFIER, "EXIT_FAILURE"},
        {Token::R_BRACKET, ")"},
        {Token::SEMICOLON, ";"},
        {Token::R_F_BRACKET, "}"},
        {Token::IDENTIFIER, "if"},
        {Token::L_BRACKET, "("},
        {Token::IDENTIFIER, "number"},
        {Token::OPERATOR, "=="},
        {Token::INSTANT, "1"},
        {Token::R_BRACKET, ")"},
        {Token::IDENTIFIER, "return"},
        {Token::INSTANT, "0"},
        {Token::SEMICOLON, ";"},
        {Token::IDENTIFIER, "if"},
        {Token::L_BRACKET, "("},
        {Token::IDENTIFIER, "number"},
        {Token::OPERATOR, "=="},
        {Token::INSTANT, "2"},
        {Token::R_BRACKET, ")"},
        {Token::IDENTIFIER, "return"},
        {Token::INSTANT, "1"},
        {Token::SEMICOLON, ";"},
        {Token::IDENTIFIER, "return"},
        {Token::IDENTIFIER, "fib"},
        {Token::L_BRACKET, "("},
        {Token::IDENTIFIER, "number"},
        {Token::OPERATOR, "-"},
        {Token::INSTANT, "1"},
        {Token::R_BRACKET, ")"},
        {Token::OPERATOR, "+"},
        {Token::IDENTIFIER, "fib"},
        {Token::L_BRACKET, "("},
        {Token::IDENTIFIER, "number"},
        {Token::OPERATOR, "-"},
        {Token::INSTANT, "2"},
        {Token::R_BRACKET, ")"},
        {Token::SEMICOLON, ";"},
        {Token::R_F_BRACKET, "}"},

        {Token::IDENTIFIER, "int"},
        {Token::IDENTIFIER, "getInput"},
        {Token::L_BRACKET, "("},
        {Token::IDENTIFIER, "void"},
        {Token::R_BRACKET, ")"},
        {Token::L_F_BRACKET, "{"},
        {Token::IDENTIFIER, "int"},
        {Token::IDENTIFIER, "num"},
        {Token::COMMA, ","},
        {Token::IDENTIFIER, "excess_len"},
        {Token::SEMICOLON, ";"},
        {Token::IDENTIFIER, "char"},
        {Token::IDENTIFIER, "buffer"},
        {Token::L_SQ_BRACKET, "["},
        {Token::INSTANT, "3"},
        {Token::R_SQ_BRACKET, "]"},
        {Token::COMMA, ","},
        {Token::OPERATOR, "*"},
        {Token::IDENTIFIER, "endPtr"},
        {Token::SEMICOLON, ";"},
        {Token::IDENTIFIER, "while"},
        {Token::L_BRACKET, "("},
        {Token::INSTANT, "1"},
        {Token::R_BRACKET, ")"},
        {Token::L_F_BRACKET, "{"},
        {Token::IDENTIFIER, "printf"},
        {Token::L_BRACKET, "("},
        {Token::STRING_INSTANT, "Please enter a valid number:"},
        {Token::R_BRACKET, ")"},
        {Token::SEMICOLON, ";"},
        {Token::IDENTIFIER, "fgets"},
        {Token::L_BRACKET, "("},
        {Token::IDENTIFIER, "buffer"},
        {Token::COMMA, ","},
        {Token::INSTANT, "3"},
        {Token::COMMA, ","},
        {Token::IDENTIFIER, "stdin"},
        {Token::R_BRACKET, ")"},
        {Token::SEMICOLON, ";"},
        {Token::IDENTIFIER, "excess_len"},
        {Token::OPERATOR, "="},
        {Token::INSTANT, "0"},
        {Token::SEMICOLON, ";"},
        {Token::IDENTIFIER, "if"},
        {Token::L_BRACKET, "("},
        {Token::OPERATOR, "!"},
        {Token::L_BRACKET, "("},
        {Token::IDENTIFIER, "buffer"},
        {Token::L_SQ_BRACKET, "["},
        {Token::INSTANT, "0"},
        {Token::R_SQ_BRACKET, "]"},
        {Token::OPERATOR, "=="},
        {Token::STRING_INSTANT, "\\n"},
        {Token::OPERATOR, "||"},
        {Token::IDENTIFIER, "buffer"},
        {Token::L_SQ_BRACKET, "["},
        {Token::INSTANT, "1"},
        {Token::R_SQ_BRACKET, "]"},
        {Token::OPERATOR, "=="},
        {Token::STRING_INSTANT, "\\n"},
        {Token::OPERATOR, "||"},
        {Token::IDENTIFIER, "buffer"},
        {Token::L_SQ_BRACKET, "["},
        {Token::INSTANT, "2"},
        {Token::R_SQ_BRACKET, "]"},
        {Token::OPERATOR, "=="},
        {Token::STRING_INSTANT, "\\n"},
        {Token::R_BRACKET, ")"},
        {Token::R_BRACKET, ")"},
        {Token::L_F_BRACKET, "{"},
        {Token::IDENTIFIER, "while"},
        {Token::L_BRACKET, "("},
        {Token::IDENTIFIER, "getchar"},
        {Token::L_BRACKET, "("},
        {Token::R_BRACKET, ")"},
        {Token::OPERATOR, "!="},
        {Token::STRING_INSTANT, "\\n"},
        {Token::R_BRACKET, ")"},
        {Token::IDENTIFIER, "excess_len"},
        {Token::OPERATOR, "++"},
        {Token::SEMICOLON, ";"},
        {Token::R_F_BRACKET, "}"},
        {Token::IDENTIFIER, "num"},
        {Token::OPERATOR, "="},
        {Token::IDENTIFIER, "strtol"},
        {Token::L_BRACKET, "("},
        {Token::IDENTIFIER, "buffer"},
        {Token::COMMA, ","},
        {Token::OPERATOR, "&"},
        {Token::IDENTIFIER, "endPtr"},
        {Token::COMMA, ","},
        {Token::INSTANT, "10"},
        {Token::R_BRACKET, ")"},
        {Token::SEMICOLON, ";"},
        {Token::IDENTIFIER, "if"},
        {Token::L_BRACKET, "("},
        {Token::L_BRACKET, "("},
        {Token::IDENTIFIER, "excess_len"},
        {Token::OPERATOR, ">"},
        {Token::INSTANT, "0"},
        {Token::OPERATOR, "||"},
        {Token::IDENTIFIER, "num"},
        {Token::OPERATOR, ">"},
        {Token::INSTANT, "48"},
        {Token::R_BRACKET, ")"},
        {Token::OPERATOR, "||"},
        {Token::L_BRACKET, "("},
        {Token::OPERATOR, "*"},
        {Token::IDENTIFIER, "endPtr"},
        {Token::OPERATOR, "!="},
        {Token::STRING_INSTANT, "\\0"},
        {Token::OPERATOR, "&&"},
        {Token::OPERATOR, "*"},
        {Token::IDENTIFIER, "endPtr"},
        {Token::OPERATOR, "!="},
        {Token::STRING_INSTANT, "\\n"},
        {Token::R_BRACKET, ")"},
        {Token::OPERATOR, "||"},
        {Token::IDENTIFIER, "endPtr"},
        {Token::OPERATOR, "=="},
        {Token::IDENTIFIER, "buffer"},
        {Token::R_BRACKET, ")"},
        {Token::L_F_BRACKET, "{"},
        {Token::IDENTIFIER, "continue"},
        {Token::SEMICOLON, ";"},
        {Token::R_F_BRACKET, "}"},
        {Token::IDENTIFIER, "break"},
        {Token::SEMICOLON, ";"},
        {Token::R_F_BRACKET, "}"},
        {Token::IDENTIFIER, "printf"},
        {Token::L_BRACKET, "("},
        {Token::STRING_INSTANT, "\\nEntered digit: %d (it might take sometime)\\n"},
        {Token::COMMA, ","},
        {Token::IDENTIFIER, "num"},
        {Token::R_BRACKET, ")"},
        {Token::SEMICOLON, ";"},
        {Token::IDENTIFIER, "return"},
        {Token::IDENTIFIER, "num"},
        {Token::SEMICOLON, ";"},
        {Token::R_F_BRACKET, "}"},

        {Token::IDENTIFIER, "static"},
        {Token::IDENTIFIER, "void"},
        {Token::IDENTIFIER, "test"},
        {Token::L_BRACKET, "("},
        {Token::R_BRACKET, ")"},
        {Token::L_F_BRACKET, "{"},
        {Token::IDENTIFIER, "assert"},
        {Token::L_BRACKET, "("},
        {Token::IDENTIFIER, "fib"},
        {Token::L_BRACKET, "("},
        {Token::INSTANT, "5"},
        {Token::R_BRACKET, ")"},
        {Token::OPERATOR, "=="},
        {Token::INSTANT, "3"},
        {Token::R_BRACKET, ")"},
        {Token::SEMICOLON, ";"},
        {Token::IDENTIFIER, "assert"},
        {Token::L_BRACKET, "("},
        {Token::IDENTIFIER, "fib"},
        {Token::L_BRACKET, "("},
        {Token::INSTANT, "2"},
        {Token::R_BRACKET, ")"},
        {Token::OPERATOR, "=="},
        {Token::INSTANT, "1"},
        {Token::R_BRACKET, ")"},
        {Token::SEMICOLON, ";"},
        {Token::IDENTIFIER, "assert"},
        {Token::L_BRACKET, "("},
        {Token::IDENTIFIER, "fib"},
        {Token::L_BRACKET, "("},
        {Token::INSTANT, "9"},
        {Token::R_BRACKET, ")"},
        {Token::OPERATOR, "=="},
        {Token::INSTANT, "21"},
        {Token::R_BRACKET, ")"},
        {Token::SEMICOLON, ";"},
        {Token::R_F_BRACKET, "}"},

        {Token::IDENTIFIER, "int"},
        {Token::IDENTIFIER, "main"},
        {Token::L_BRACKET, "("},
        {Token::R_BRACKET, ")"},
        {Token::L_F_BRACKET, "{"},
        {Token::IDENTIFIER, "test"},
        {Token::L_BRACKET, "("},
        {Token::R_BRACKET, ")"},
        {Token::SEMICOLON, ";"},
        {Token::IDENTIFIER, "printf"},
        {Token::L_BRACKET, "("},
        {Token::STRING_INSTANT, "Tests passed...\\n"},
        {Token::R_BRACKET, ")"},
        {Token::SEMICOLON, ";"},
        {Token::IDENTIFIER, "printf"},
        {Token::L_BRACKET, "("},
        {Token::STRING_INSTANT, "Enter n to find nth fibonacci element...\\n"},
        {Token::STRING_INSTANT, "Note: You would be asked to enter input until valid number ( less "},
        {Token::STRING_INSTANT, "than or equal to 48 ) is entered.\\n"},
        {Token::R_BRACKET, ")"},
        {Token::SEMICOLON, ";"},
        {Token::IDENTIFIER, "int"},
        {Token::IDENTIFIER, "number"},
        {Token::OPERATOR, "="},
        {Token::IDENTIFIER, "getInput"},
        {Token::L_BRACKET, "("},
        {Token::R_BRACKET, ")"},
        {Token::SEMICOLON, ";"},
        {Token::IDENTIFIER, "clock_t"},
        {Token::IDENTIFIER, "start"},
        {Token::COMMA, ","},
        {Token::IDENTIFIER, "end"},
        {Token::SEMICOLON, ";"},
        {Token::IDENTIFIER, "start"},
        {Token::OPERATOR, "="},
        {Token::IDENTIFIER, "clock"},
        {Token::L_BRACKET, "("},
        {Token::R_BRACKET, ")"},
        {Token::SEMICOLON, ";"},
        {Token::IDENTIFIER, "printf"},
        {Token::L_BRACKET, "("},
        {Token::STRING_INSTANT, "Fibonacci element %d is %u "},
        {Token::COMMA, ","},
        {Token::IDENTIFIER, "number"},
        {Token::COMMA, ","},
        {Token::IDENTIFIER, "fib"},
        {Token::L_BRACKET, "("},
        {Token::IDENTIFIER, "number"},
        {Token::R_BRACKET, ")"},
        {Token::R_BRACKET, ")"},
        {Token::SEMICOLON, ";"},
        {Token::IDENTIFIER, "end"},
        {Token::OPERATOR, "="},
        {Token::IDENTIFIER, "clock"},
        {Token::L_BRACKET, "("},
        {Token::R_BRACKET, ")"},
        {Token::SEMICOLON, ";"},
        {Token::IDENTIFIER, "printf"},
        {Token::L_BRACKET, "("},
        {Token::STRING_INSTANT, "in %.3f seconds.\\n"},
        {Token::COMMA, ","},
        {Token::L_BRACKET, "("},
        {Token::L_BRACKET, "("},
        {Token::IDENTIFIER, "double"},
        {Token::R_BRACKET, ")"},
        {Token::L_BRACKET, "("},
        {Token::IDENTIFIER, "end"},
        {Token::OPERATOR, "-"},
        {Token::IDENTIFIER, "start"},
        {Token::R_BRACKET, ")"},
        {Token::R_BRACKET, ")"},
        {Token::OPERATOR, "/"},
        {Token::IDENTIFIER, "CLOCKS_PER_SEC"},
        {Token::R_BRACKET, ")"},
        {Token::SEMICOLON, ";"},
        {Token::IDENTIFIER, "return"},
        {Token::INSTANT, "0"},
        {Token::SEMICOLON, ";"},
        {Token::R_F_BRACKET, "}"},
    };

    auto compare_tokens = [](const Token& a, const Token& b) {
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
    Tokenizer tokenizer;
    auto r = tokenizer.tokenize("123.2 //comment till end of line//full line comment\n132.2");
    ASSERT_EQ(r.error, "");
    ASSERT_EQ(r.value.size(), 2);
    EXPECT_EQ(r.value[0].t, Token::INSTANT);
    EXPECT_EQ(r.value[0].data, "123.2");
    EXPECT_EQ(r.value[1].t, Token::INSTANT);
    EXPECT_EQ(r.value[1].data, "132.2");
}

TEST(TokenizerTest, IgnoreSpacesTabsLineBreaks) {
    Tokenizer tokenizer;
    auto r = tokenizer.tokenize("123.2  132.2 \t \t 1 12\t\t1");
    ASSERT_EQ(r.error, "");
    ASSERT_EQ(r.value.size(), 5);
    EXPECT_EQ(r.value[0].t, Token::INSTANT);
    EXPECT_EQ(r.value[0].data, "123.2");
    EXPECT_EQ(r.value[1].t, Token::INSTANT);
    EXPECT_EQ(r.value[1].data, "132.2");
    EXPECT_EQ(r.value[2].t, Token::INSTANT);
    EXPECT_EQ(r.value[2].data, "1");
    EXPECT_EQ(r.value[3].t, Token::INSTANT);
    EXPECT_EQ(r.value[3].data, "12");
    EXPECT_EQ(r.value[4].t, Token::INSTANT);
    EXPECT_EQ(r.value[4].data, "1");
}

TEST(TokenizerTest, Identifiers) {
    Tokenizer tokenizer;
    auto r = tokenizer.tokenize("test_123 test-123 _test 123test");
    ASSERT_EQ(r.error, "");
    ASSERT_EQ(r.value.size(), 5);
    EXPECT_EQ(r.value[0].t, Token::IDENTIFIER);
    EXPECT_EQ(r.value[0].data, "test_123");
    EXPECT_EQ(r.value[1].t, Token::IDENTIFIER);
    EXPECT_EQ(r.value[1].data, "test-123");
    EXPECT_EQ(r.value[2].t, Token::IDENTIFIER);
    EXPECT_EQ(r.value[2].data, "_test");
    EXPECT_EQ(r.value[3].t, Token::INSTANT);
    EXPECT_EQ(r.value[3].data, "123");
    EXPECT_EQ(r.value[4].t, Token::IDENTIFIER);
    EXPECT_EQ(r.value[4].data, "test");
}

TEST(TokenizerTest, StringLiterals) {
    Tokenizer tokenizer;
    auto r = tokenizer.tokenize(R"("hello world")");
    ASSERT_EQ(r.error, "");
    ASSERT_EQ(r.value.size(), 1);
    EXPECT_EQ(r.value[0].t, Token::STRING_INSTANT);
    EXPECT_EQ(r.value[0].data, "hello world");
}

TEST(TokenizerTest, EmptyString) {
    Tokenizer tokenizer;
    auto r = tokenizer.tokenize(R"("")");
    ASSERT_EQ(r.error, "");
    ASSERT_EQ(r.value.size(), 1);
    EXPECT_EQ(r.value[0].t, Token::STRING_INSTANT);
    EXPECT_EQ(r.value[0].data, "");
}

TEST(TokenizerTest, UnterminatedStringError) {
    Tokenizer tokenizer;
    auto r = tokenizer.tokenize(R"("unterminated)");
    EXPECT_NE(r.error, "");
}

TEST(TokenizerTest, ComplexOperators) {
    Tokenizer tokenizer;
    std::string src = "!= == <<= >>= += -= *= /= %= &= |= ^= ~";
    auto res = tokenizer.tokenize(src);
    ASSERT_EQ(res.error, "");
    std::vector<std::string> expected = {"!=", "==", "<<=", ">>=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "~"};
    ASSERT_EQ(res.value.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(res.value[i].t, Token::OPERATOR);
        EXPECT_EQ(res.value[i].data, expected[i]);
    }
}

TEST(TokenizerTest, SlashOperatorsVsComments) {
    Tokenizer tokenizer;
    // Ensure '//' starts comment, but '/=' is operator
    auto r = tokenizer.tokenize("/= // this is comment\n*=");
    ASSERT_EQ(r.error, "");
    ASSERT_EQ(r.value.size(), 2);
    EXPECT_EQ(r.value[0].data, "/=");
    EXPECT_EQ(r.value[1].data, "*=");
}

TEST(TokenizerTest, NumberWithSuffixes) {
    Tokenizer tokenizer;
    auto r = tokenizer.tokenize("123u 456l 789i 10.5u");
    ASSERT_EQ(r.error, "");
    ASSERT_EQ(r.value.size(), 4);
    EXPECT_EQ(r.value[0].data, "123u");
    EXPECT_EQ(r.value[1].data, "456l");
    EXPECT_EQ(r.value[2].data, "789i");
    EXPECT_EQ(r.value[3].data, "10.5u");  // Note: your tokenizer allows suffixes on floats too
    for (const auto& t : r.value) {
        EXPECT_EQ(t.t, Token::INSTANT);
    }
}

TEST(TokenizerTest, IdentifierStartsWithDigit) {
    Tokenizer tokenizer;
    // "123abc" -> "123" (INSTANT) + "abc" (IDENTIFIER)
    auto r = tokenizer.tokenize("123abc");
    ASSERT_EQ(r.error, "");
    ASSERT_EQ(r.value.size(), 2);
    EXPECT_EQ(r.value[0].t, Token::INSTANT);
    EXPECT_EQ(r.value[0].data, "123");
    EXPECT_EQ(r.value[1].t, Token::IDENTIFIER);
    EXPECT_EQ(r.value[1].data, "abc");
}

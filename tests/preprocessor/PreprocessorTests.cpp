
#include <gtest/gtest.h>

#include "frontend/lexic/preprocessor_tokenizer.h"

auto compare_tokens = [](const eraxc::token& a, const eraxc::token& b) {
    if (a.t == b.t && a.data == b.data) {
        return testing::AssertionSuccess();
    }
    return testing::AssertionFailure() << a.data << " != " << b.data;
};

TEST(PreprocessorTest, define_ifdef_ifndef) {
    std::string src = "#define test\n"
                      "#define jle jg\n"
                      "#ifdef test\n123\n#endif\n"
                      "#ifndef test\n321\n#endif\n"
                      "#ifndef test1\ntest11\n#endif\n";

    const std::vector<eraxc::token> ref {{eraxc::token::INSTANT, "123"}, {eraxc::token::IDENTIFIER, "test11"}};

    eraxc::tokenizer tokenizer;
    auto res = tokenizer.tokenize(src);
    ASSERT_EQ(res.error, "");
    ASSERT_EQ(res.value.size(), ref.size());

    for (auto i = 0u; i < ref.size(); i++) {
        compare_tokens(res.value[i], ref[i]);
    }
}

TEST(PreprocessorTest, define_change_all_occurences) {
    std::string src = "#define jle jg\n"
                      "#ifdef jle\njle\n#endif\n"
                      "jle1 jle 1jle jle_";

    const std::vector<eraxc::token> ref {
        {eraxc::token::IDENTIFIER, "jg"}, {eraxc::token::IDENTIFIER, "jle1"}, {eraxc::token::IDENTIFIER, "jg"},
        {eraxc::token::INSTANT, "1"},     {eraxc::token::IDENTIFIER, "jg"},   {eraxc::token::IDENTIFIER, "jle_"},
    };

    eraxc::tokenizer tokenizer;
    auto res = tokenizer.tokenize(src);
    ASSERT_EQ(res.error, "");
    ASSERT_EQ(res.value.size(), ref.size());

    for (auto i = 0u; i < ref.size(); i++) {
        compare_tokens(res.value[i], ref[i]);
    }
}

TEST(PreprocessorTest, DefineWithoutValue) {
    std::string src = "#define FLAG\n#ifdef FLAG\nx\n#endif";
    eraxc::tokenizer tokenizer;
    auto res = tokenizer.tokenize(src);
    ASSERT_EQ(res.error, "");
    ASSERT_EQ(res.value.size(), 1);
    EXPECT_EQ(res.value[0].t, eraxc::token::IDENTIFIER);
    EXPECT_EQ(res.value[0].data, "x");
}

TEST(PreprocessorTest, MacroSubstitutionFullMatchOnly) {
    std::string src = "#define A B\nA AA A1 _A";
    eraxc::tokenizer tokenizer;
    auto res = tokenizer.tokenize(src);
    ASSERT_EQ(res.error, "");
    std::vector<eraxc::token> expected = {{eraxc::token::IDENTIFIER, "B"},
                                          {eraxc::token::IDENTIFIER, "AA"},
                                          {eraxc::token::IDENTIFIER, "A1"},
                                          {eraxc::token::IDENTIFIER, "_A"}};
    ASSERT_EQ(res.value.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_TRUE(compare_tokens(res.value[i], expected[i]));
    }
}

TEST(PreprocessorTest, NestedIfdef) {
    std::string src = "#define A\n#ifdef A\n#define B\n#ifdef B\nresult\n#endif\n#endif";
    eraxc::tokenizer tokenizer;
    auto res = tokenizer.tokenize(src);
    ASSERT_EQ(res.error, "");
    ASSERT_EQ(res.value.size(), 1);
    EXPECT_EQ(res.value[0].data, "result");
}

TEST(PreprocessorTest, NestedIfdefWithNesting) {
    std::string src = R"(
#define A
#ifdef A
#define B
#ifdef B
42
#endif
#endif
)";
    eraxc::tokenizer t;
    auto res = t.tokenize(src);
    ASSERT_EQ(res.error, "");
    ASSERT_EQ(res.value.size(), 1);
    EXPECT_EQ(res.value[0].t, eraxc::token::INSTANT);
    EXPECT_EQ(res.value[0].data, "42");
}

TEST(PreprocessorTest, MissingEndifError) {
    std::string src = "#ifdef TEST\n123\n";  // no #endif
    eraxc::tokenizer tokenizer;
    auto res = tokenizer.tokenize(src);
    EXPECT_NE(res.error, "");
    EXPECT_TRUE(res.error.find("expected #endif") != std::string::npos);
}

TEST(PreprocessorTest, DefineInvalidIdentifier) {
    std::string src = "#define 123 x";  // identifier can't start with digit
    eraxc::tokenizer tokenizer;
    auto res = tokenizer.tokenize(src);
    EXPECT_NE(res.error, "");
    EXPECT_TRUE(res.error.find("expected identifier") != std::string::npos);
}

TEST(PreprocessorTest, DefineSecondArgNotIdentifier) {
    std::string src = "#define A 123!";  // '!' is not allowed in replacement
    eraxc::tokenizer tokenizer;
    auto res = tokenizer.tokenize(src);
    EXPECT_NE(res.error, "");
    EXPECT_TRUE(res.error.find("Expected identifier as define second argument") != std::string::npos);
}

TEST(PreprocessorTest, UnknownDirective) {
    std::string src = "#unknown\n123";
    eraxc::tokenizer tokenizer;
    auto res = tokenizer.tokenize(src);
    EXPECT_NE(res.error, "");
    EXPECT_TRUE(res.error.find("No such macro") != std::string::npos);
}

#include <gtest/gtest.h>

#include "frontend/lexic/preprocessor_tokenizer.h"

TEST(TokenizerTest, ParseNumbers) {
    eraxc::tokenizer tokenizer;
    auto r = tokenizer.tokenize("123");
    ASSERT_EQ(r.error, "");
    EXPECT_EQ(r.value.size(), 1);
    EXPECT_EQ(r.value[0].t, eraxc::token::INSTANT);
    EXPECT_EQ(r.value[0].data, "123");
}

TEST(TokenizerTest, ParseFloatNumbers) {
    eraxc::tokenizer tokenizer;
    auto r = tokenizer.tokenize("123.2");
    ASSERT_EQ(r.error, "");
    EXPECT_EQ(r.value.size(), 1);
    EXPECT_EQ(r.value[0].t, eraxc::token::INSTANT);
    EXPECT_EQ(r.value[0].data, "123.2");
}

TEST(TokenizerTest, ParseSingleCharOperators) {
    eraxc::tokenizer tokenizer;
    auto checkResult = [&tokenizer](const std::string& src, char op) {
        auto res = tokenizer.tokenize(src);
        ASSERT_EQ(res.error, "");
        EXPECT_EQ(res.value.size(), 1);
        EXPECT_EQ(res.value[0].data, std::to_string(op));
        EXPECT_EQ(res.value[0].t, eraxc::token::OPERATOR);
    };

    for (const auto& op : eraxc::token::operator_chars) { checkResult(std::to_string(op), op); }
}

TEST(TokenizerTest, ParseMultiCharOperators) {
    eraxc::tokenizer tokenizer;
    auto checkResult = [&tokenizer](const std::string& src, const std::string& op) {
        auto res = tokenizer.tokenize(src);
        ASSERT_EQ(res.error, "");
        EXPECT_EQ(res.value.size(), 1);
        EXPECT_EQ(res.value[0].data, op);
        EXPECT_EQ(res.value[0].t, eraxc::token::OPERATOR);
    };

    for (const auto& op : eraxc::token::operator_chars) {
        auto ops = std::to_string(op) + std::to_string(op) + std::to_string(op);  //triple operator!
        checkResult(ops, ops);
    }
}
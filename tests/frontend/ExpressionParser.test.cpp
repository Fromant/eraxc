#include "frontend/syntax/parser/ExpressionParser.hpp"

#include <gtest/gtest.h>

#include "frontend/lexic/PreprocessorTokenizer.hpp"
#include "frontend/syntax/scope/ScopeManager.hpp"

using namespace eraxc::frontend;

class ExpressionParserTest : public ::testing::Test {
protected:
    ScopeManager scopeManager;
    ExpressionParser parser {scopeManager};

    ExpressionParserTest() {
        const auto type = scopeManager.findTypeRecursive("i32");
        scopeManager.addId("a", type.value(), false, false);
        scopeManager.addId("b", type.value(), false, false);
        scopeManager.addId("c", type.value(), false, false);
        scopeManager.addId("d", type.value(), false, false);
        scopeManager.addId("e", type.value(), false, false);
        scopeManager.addId("f", type.value(), false, false);
        scopeManager.addId("g", type.value(), false, false);
        scopeManager.addId("h", type.value(), false, false);
        scopeManager.addId("i", type.value(), false, false);
        scopeManager.addId("j", type.value(), false, false);
        scopeManager.addId("k", type.value(), false, false);
        scopeManager.addId("l", type.value(), false, false);
        scopeManager.addId("foo", type.value(), true, false);
        scopeManager.addId("bar", type.value(), true, false);
    }
    static std::vector<Token> tokenize(const std::string& expr) {
        Tokenizer t {};
        auto res = t.tokenize(expr);
        if (!res) {
            throw std::runtime_error("Test failure");
        }
        return res.value;
    }

    // Helper to extract operator sequence from RPN output for comparison
    std::string rpnToString(const std::vector<RPNToken>& rpn) {
        std::string result;
        for (const auto& token : rpn) {
            if (!result.empty())
                result += " ";
            switch (token.type) {
                case RPNToken::Type::Identifier: result += "ID:" + std::to_string(token.payload.id); break;
                case RPNToken::Type::LiteralInt: result += "LIT:" + std::to_string(token.payload.int_val); break;
                case RPNToken::Type::LiteralFloat: result += "LITF:" + std::to_string(token.payload.float_val); break;
                case RPNToken::Type::Operator:
                    // Map operator types back to strings for readability
                    switch (token.payload.operator_type) {
                        case OperatorType::ADD: result += "ADD"; break;
                        case OperatorType::SUBTRACT: result += "SUB"; break;
                        case OperatorType::MULTIPLY: result += "MUL"; break;
                        case OperatorType::DIVIDE: result += "DIV"; break;
                        case OperatorType::NEGATIVE: result += "NEG"; break;
                        case OperatorType::POSITIVE: result += "POS"; break;
                        case OperatorType::NOT: result += "NOT"; break;
                        case OperatorType::BITWISE_NOT: result += "BNOT"; break;
                        case OperatorType::INCREMENT: result += "INC"; break;
                        case OperatorType::DECREMENT: result += "DEC"; break;
                        case OperatorType::ADDRESSOF: result += "ADDR"; break;
                        case OperatorType::INDIRECTION: result += "DEREF"; break;
                        case OperatorType::CALL: result += "CALL"; break;
                        case OperatorType::ASSIGN: result += "ASSIGN"; break;
                        case OperatorType::EQUAL: result += "EQ"; break;
                        case OperatorType::AND: result += "AND"; break;
                        case OperatorType::OR: result += "OR"; break;
                        default: result += "OP_" + std::to_string((int)token.payload.operator_type);
                    }
                    break;
                case RPNToken::Type::Keyword: result += "KW:" + std::to_string((int)token.payload.keyword); break;
                default: result += "UNK";
            }
        }
        return result;
    }
};

// === BASIC EXPRESSION TESTS ===

TEST_F(ExpressionParserTest, SimpleBinaryExpression) {
    auto tokens = tokenize("a + b");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    EXPECT_EQ(rpnToString(result.value), "ID:1 ID:2 ADD");
}

TEST_F(ExpressionParserTest, Precedence_MultiplicationBeforeAddition) {
    auto tokens = tokenize("a + b * c");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    EXPECT_EQ(rpnToString(result.value), "ID:1 ID:2 ID:3 MUL ADD");
}

TEST_F(ExpressionParserTest, Precedence_ParenthesesOverride) {
    auto tokens = tokenize("(a + b) * c");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    EXPECT_EQ(rpnToString(result.value), "ID:1 ID:2 ADD ID:3 MUL");
}

TEST_F(ExpressionParserTest, RightAssociative_Assignment) {
    auto tokens = tokenize("a = b = c");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    // Right-associative: a = (b = c) => b c ASSIGN a ASSIGN
    EXPECT_EQ(rpnToString(result.value), "ID:2 ID:3 ASSIGN ID:1 ASSIGN");
}

// === PREFIX OPERATOR TESTS ===

TEST_F(ExpressionParserTest, Prefix_Negation) {
    auto tokens = tokenize("-a");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    EXPECT_EQ(rpnToString(result.value), "ID:1 NEG");
}

TEST_F(ExpressionParserTest, Prefix_LogicalNot) {
    auto tokens = tokenize("!a");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    EXPECT_EQ(rpnToString(result.value), "ID:1 NOT");
}

TEST_F(ExpressionParserTest, Prefix_BitwiseNot) {
    auto tokens = tokenize("~a");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    EXPECT_EQ(rpnToString(result.value), "ID:1 BNOT");
}

TEST_F(ExpressionParserTest, Prefix_AddressOf) {
    auto tokens = tokenize("&a");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    EXPECT_EQ(rpnToString(result.value), "ID:1 ADDR");
}

TEST_F(ExpressionParserTest, Prefix_Dereference) {
    auto tokens = tokenize("*a");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    EXPECT_EQ(rpnToString(result.value), "ID:1 DEREF");
}

TEST_F(ExpressionParserTest, Prefix_Increment) {
    auto tokens = tokenize("++a");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    EXPECT_EQ(rpnToString(result.value), "ID:1 INC");
}

TEST_F(ExpressionParserTest, Prefix_Decrement) {
    auto tokens = tokenize("--a");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    EXPECT_EQ(rpnToString(result.value), "ID:1 DEC");
}

TEST_F(ExpressionParserTest, Prefix_ConsecutiveOperators) {
    auto tokens = tokenize("- !a");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    // Should parse as -( !x ) => x NOT NEG
    EXPECT_EQ(rpnToString(result.value), "ID:1 NOT NEG");
}

TEST_F(ExpressionParserTest, Prefix_InExpression_Context) {
    auto tokens = tokenize("a + -b");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    // The '-' after '+' should be prefix negation, not binary subtract
    EXPECT_EQ(rpnToString(result.value), "ID:1 ID:2 NEG ADD");
}

TEST_F(ExpressionParserTest, Prefix_WithParentheses) {
    auto tokens = tokenize("-(a + b)");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    EXPECT_EQ(rpnToString(result.value), "ID:1 ID:2 ADD NEG");
}

// === POSTFIX OPERATOR TESTS ===

TEST_F(ExpressionParserTest, Postfix_Increment) {
    auto tokens = tokenize("a++");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    EXPECT_EQ(rpnToString(result.value), "ID:1 INC");
}

TEST_F(ExpressionParserTest, Postfix_Decrement) {
    auto tokens = tokenize("a--");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    EXPECT_EQ(rpnToString(result.value), "ID:1 DEC");
}

TEST_F(ExpressionParserTest, Postfix_InExpression) {
    auto tokens = tokenize("a++ + b");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    // Postfix ++ binds to x before the addition
    EXPECT_EQ(rpnToString(result.value), "ID:1 INC ID:2 ADD");
}

// === PREFIX vs POSTFIX DISAMBIGUATION ===

TEST_F(ExpressionParserTest, Disambiguation_PlusMinus) {
    // Binary + vs prefix +
    auto tokens1 = tokenize("a + +b");
    size_t pos1 = 0;
    auto result1 = parser.parse(tokens1, pos1);
    ASSERT_TRUE(result1);
    EXPECT_EQ(rpnToString(result1.value), "ID:1 ID:2 POS ADD");

    // Binary - vs prefix -
    auto tokens2 = tokenize("a - -b");
    size_t pos2 = 0;
    auto result2 = parser.parse(tokens2, pos2);
    ASSERT_TRUE(result2);
    EXPECT_EQ(rpnToString(result2.value), "ID:1 ID:2 NEG SUB");
}

TEST_F(ExpressionParserTest, Disambiguation_Star) {
    // Binary * vs prefix dereference
    auto tokens1 = tokenize("a * b");
    size_t pos1 = 0;
    auto result1 = parser.parse(tokens1, pos1);
    ASSERT_TRUE(result1);
    EXPECT_EQ(rpnToString(result1.value), "ID:1 ID:2 MUL");

    auto tokens2 = tokenize("*a");
    size_t pos2 = 0;
    auto result2 = parser.parse(tokens2, pos2);
    ASSERT_TRUE(result2);
    EXPECT_EQ(rpnToString(result2.value), "ID:1 DEREF");
}

TEST_F(ExpressionParserTest, Disambiguation_Ampersand) {
    // Binary & vs prefix address-of
    auto tokens1 = tokenize("a & b");
    size_t pos1 = 0;
    auto result1 = parser.parse(tokens1, pos1);
    ASSERT_TRUE(result1);
    EXPECT_EQ(rpnToString(result1.value), "ID:1 ID:2 OP_16");

    auto tokens2 = tokenize("&a");
    size_t pos2 = 0;
    auto result2 = parser.parse(tokens2, pos2);
    ASSERT_TRUE(result2);
    EXPECT_EQ(rpnToString(result2.value), "ID:1 ADDR");
}

TEST_F(ExpressionParserTest, Disambiguation_IncrementDecrement) {
    // Prefix vs postfix ++
    auto tokens1 = tokenize("++a");
    size_t pos1 = 0;
    auto result1 = parser.parse(tokens1, pos1);
    ASSERT_TRUE(result1);
    EXPECT_EQ(rpnToString(result1.value), "ID:1 INC");

    auto tokens2 = tokenize("a++");
    size_t pos2 = 0;
    auto result2 = parser.parse(tokens2, pos2);
    ASSERT_TRUE(result2);
    EXPECT_EQ(rpnToString(result2.value), "ID:1 INC");

    // Note: Both produce same RPN; semantic difference (pre vs post)
    // would be handled at codegen with different instruction selection
}

// === COMPLEX EXPRESSIONS ===

TEST_F(ExpressionParserTest, Complex_MixedUnaryBinary) {
    auto tokens = tokenize("a + -b * ++c");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    // Precedence: ++c (postfix), then -b (prefix), then *, then +
    // RPN: a b NEG c INC MUL ADD
    EXPECT_EQ(rpnToString(result.value), "ID:1 ID:2 NEG ID:3 INC MUL ADD");
}

TEST_F(ExpressionParserTest, Complex_NestedParentheses) {
    auto tokens = tokenize("-(a + !(b > c))");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    // Inner: b c GT, then NOT, then a + result, then NEG
    EXPECT_EQ(rpnToString(result.value), "ID:2 ID:3 OP_7 NOT ID:1 ADD NEG");
}

TEST_F(ExpressionParserTest, Complex_ChainedComparisons) {
    auto tokens = tokenize("a < b && b < c");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    // && has lower precedence than <, so: (a<b) && (b<c)
    EXPECT_EQ(rpnToString(result.value), "ID:1 ID:2 OP_9 ID:2 ID:3 OP_9 AND");
}

// === FUNCTION CALLS ===

TEST_F(ExpressionParserTest, FunctionCall_NoArgs) {
    auto tokens = tokenize("foo()");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    EXPECT_EQ(rpnToString(result.value), "ID:13 CALL");
}

TEST_F(ExpressionParserTest, FunctionCall_WithArgs) {
    auto tokens = tokenize("foo(a, b)");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    EXPECT_EQ(rpnToString(result.value), "ID:1 ID:2 ID:13 CALL");
}

TEST_F(ExpressionParserTest, FunctionCall_WithUnaryArgs) {
    auto tokens = tokenize("foo(-a, !b)");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    EXPECT_EQ(rpnToString(result.value), "ID:1 NEG ID:2 NOT ID:13 CALL");
}

TEST_F(ExpressionParserTest, FunctionCall_Nested) {
    auto tokens = tokenize("foo(bar(a))");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    // Inner call first: x inner CALL, then outer CALL
    EXPECT_EQ(rpnToString(result.value), "ID:1 ID:13 CALL ID:14 CALL");
}

// === EDGE CASES ===

TEST_F(ExpressionParserTest, Edge_SingleOperand) {
    auto tokens = tokenize("a");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    EXPECT_EQ(rpnToString(result.value), "ID:1");
}

TEST_F(ExpressionParserTest, Edge_SingleLiteral) {
    auto tokens = tokenize("42");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    EXPECT_EQ(rpnToString(result.value), "LIT:42");
}

TEST_F(ExpressionParserTest, Edge_BooleanLiterals) {
    auto tokens = tokenize("true && false");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    // Keywords TRUE/FALSE should be recognized
    EXPECT_TRUE(rpnToString(result.value).find("KW:") != std::string::npos);
}

TEST_F(ExpressionParserTest, Edge_MultipleUnaryPrefix) {
    auto tokens = tokenize("! ! !a");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    // Right-to-left: !(!(!x)) => x NOT NOT NOT
    EXPECT_EQ(rpnToString(result.value), "ID:1 NOT NOT NOT");
}

TEST_F(ExpressionParserTest, Edge_UnaryAfterParen) {
    auto tokens = tokenize("(a + b)++");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    // Postfix on parenthesized expression
    EXPECT_EQ(rpnToString(result.value), "ID:1 ID:2 ADD INC");
}

TEST_F(ExpressionParserTest, Edge_PrefixBeforeParen) {
    auto tokens = tokenize("-(a + b)");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    EXPECT_EQ(rpnToString(result.value), "ID:1 ID:2 ADD NEG");
}

// === ERROR CASES ===

TEST_F(ExpressionParserTest, Error_MismatchedParen_Open) {
    auto tokens = tokenize("(a + b");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_FALSE(result);
    EXPECT_NE(result.error.find("paren"), std::string::npos) << result.error;
}

TEST_F(ExpressionParserTest, Error_MismatchedParen_Close) {
    auto tokens = tokenize("a + b)");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_FALSE(result);
    EXPECT_NE(result.error.find("paren"), std::string::npos) << result.error;
}

TEST_F(ExpressionParserTest, Error_UnknownOperator) {
    auto tokens = tokenize("a @ b");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_FALSE(result);
    EXPECT_NE(result.error.find("operator"), std::string::npos) << result.error;
}

TEST_F(ExpressionParserTest, Error_UnexpectedComma) {
    auto tokens = tokenize("a, b");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos, {Token::SEMICOLON});

    ASSERT_FALSE(result);
    EXPECT_NE(result.error.find("comma"), std::string::npos) << result.error;
}

// === TERMINATION PARAMETER TESTS ===

TEST_F(ExpressionParserTest, Termination_Semicolon) {
    auto tokens = tokenize("a + b; c");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos, {Token::SEMICOLON});

    ASSERT_TRUE(result) << result.error;
    // Should stop at semicolon, not parse "c"
    EXPECT_EQ(rpnToString(result.value), "ID:1 ID:2 ADD");
    EXPECT_EQ(tokens[pos].t, Token::SEMICOLON);  // pos should be at semicolon
}

TEST_F(ExpressionParserTest, Termination_RightParen) {
    auto tokens = tokenize("a + b) rest");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos, {Token::R_BRACKET});

    ASSERT_TRUE(result) << result.error;
    EXPECT_EQ(rpnToString(result.value), "ID:1 ID:2 ADD");
    EXPECT_EQ(tokens[pos].t, Token::R_BRACKET);
}

// === PRECEDENCE VERIFICATION ===

TEST_F(ExpressionParserTest, Precedence_FullHierarchy) {
    // Test that all precedence levels work correctly
    // From highest to lowest: postfix, unary, */%, +-, <<>>, <<=, ==, &, ^, |, &&, ||, =
    auto tokens = tokenize("a = b || c && d ^ e | f & g == h < i << j + k * l");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    // Just verify it parses without error and produces reasonable output length
    EXPECT_GT(result.value.size(), 10);
    // The last operator in RPN should be ASSIGN (lowest precedence)
    EXPECT_EQ(result.value.back().type, RPNToken::Type::Operator);
    EXPECT_EQ(result.value.back().payload.operator_type, OperatorType::ASSIGN);
}
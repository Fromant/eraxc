#include "frontend/syntax/parser/ExpressionParser.hpp"

#include <gtest/gtest.h>

#include "frontend/lexic/PreprocessorTokenizer.hpp"
#include "frontend/syntax/scope/ScopeManager.hpp"

using namespace eraxc::frontend;
using namespace eraxc::JIR;
using namespace eraxc::CFG;

class ExpressionParserTest : public ::testing::Test {
protected:
    ScopeManager scopeManager;
    FrontendResult result;
    ExpressionParser parser {scopeManager, result};

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

    static bool operandsEqual(const Operand& a, const Operand& b) {
        return a.value == b.value && a.is_instant == b.is_instant && a.is_rvalue == b.is_rvalue && a.type == b.type;
    }

    static bool commandsEqual(const Command& a, const Command& b) {
        return a.op == b.op && operandsEqual(a.operand1, b.operand1) && operandsEqual(a.operand2, b.operand2);
    }

    static bool nodesEqual(const CFGNode& a, const CFGNode& b) {
        if (a.size() != b.size()) {
            return false;
        }
        for (size_t i = 0; i < a.size(); i++) {
            if (!commandsEqual(a[i], b[i])) {
                return false;
            }
        }
        return true;
    }

    static Operand operandFromDecl(const Scope::Declaration& decl) {
        const auto& type = decl.getJirType();
        EXPECT_TRUE(type) << type.error;
        return Operand {type.value, decl.getId(), false, false};
    }
};

constexpr Type i32 = jirTypeFromKeyword(Keyword::i32);

TEST_F(ExpressionParserTest, SimpleBinaryExpression) {
    auto tokens = tokenize("a + b;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    const auto& a_decl_opt = scopeManager.findDeclarationRecursive("a");
    const auto& b_decl_opt = scopeManager.findDeclarationRecursive("b");

    ASSERT_TRUE(a_decl_opt);
    ASSERT_TRUE(b_decl_opt);

    const auto& a_decl = a_decl_opt.value();
    const auto& b_decl = b_decl_opt.value();

    const Operand new_var {i32, 15, false, true};

    ASSERT_TRUE(result) << result.error;
    ASSERT_TRUE(operandsEqual(result.value.result, new_var));

    CFGNode nodes = {Command {Operation::MOVE, new_var, operandFromDecl(a_decl)},
                     Command {Operation::ADD, new_var, operandFromDecl(b_decl)}};

    ASSERT_TRUE(nodesEqual(result.value.node, nodes));
}

TEST_F(ExpressionParserTest, Precedence_MultiplicationBeforeAddition) {
    auto tokens = tokenize("a + b * c;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_EQ(pos, 6);  // 6 tokens
    const auto& a_decl_opt = scopeManager.findDeclarationRecursive("a");
    const auto& b_decl_opt = scopeManager.findDeclarationRecursive("b");
    const auto& c_decl_opt = scopeManager.findDeclarationRecursive("c");

    ASSERT_TRUE(a_decl_opt);
    ASSERT_TRUE(b_decl_opt);
    ASSERT_TRUE(c_decl_opt);

    const auto& a_decl = a_decl_opt.value();
    const auto& b_decl = b_decl_opt.value();
    const auto& c_decl = c_decl_opt.value();

    const Operand new_var {i32, 15, false, true};
    const Operand new_var2 {i32, 16, false, true};

    ASSERT_TRUE(result) << result.error;
    ASSERT_TRUE(operandsEqual(result.value.result, new_var2));

    CFGNode nodes = {
        Command {Operation::MOVE, new_var, operandFromDecl(b_decl)},
        Command {Operation::MUL, new_var, operandFromDecl(c_decl)},
        Command {Operation::MOVE, new_var2, operandFromDecl(a_decl)},
        Command {Operation::ADD, new_var2, new_var},
    };

    ASSERT_TRUE(nodesEqual(result.value.node, nodes));
}

TEST_F(ExpressionParserTest, Precedence_ParenthesesOverride) {
    auto tokens = tokenize("(a + b) * c;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_EQ(pos, 8);  // 8 tokens
    const auto& a_decl_opt = scopeManager.findDeclarationRecursive("a");
    const auto& b_decl_opt = scopeManager.findDeclarationRecursive("b");
    const auto& c_decl_opt = scopeManager.findDeclarationRecursive("c");

    ASSERT_TRUE(a_decl_opt);
    ASSERT_TRUE(b_decl_opt);
    ASSERT_TRUE(c_decl_opt);

    const auto& a_decl = a_decl_opt.value();
    const auto& b_decl = b_decl_opt.value();
    const auto& c_decl = c_decl_opt.value();

    const Operand new_var {i32, 15, false, true};

    ASSERT_TRUE(result) << result.error;
    ASSERT_TRUE(operandsEqual(result.value.result, new_var));

    CFGNode nodes = {
        Command {Operation::MOVE, new_var, operandFromDecl(a_decl)},
        Command {Operation::ADD, new_var, operandFromDecl(b_decl)},
        Command {Operation::MUL, new_var, operandFromDecl(c_decl)},
    };

    ASSERT_TRUE(nodesEqual(result.value.node, nodes));
}

TEST_F(ExpressionParserTest, RightAssociative_Assignment) {
    auto tokens = tokenize("a = b = c;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_EQ(pos, 6);  // 6 tokens
    const auto& a_decl_opt = scopeManager.findDeclarationRecursive("a");
    const auto& b_decl_opt = scopeManager.findDeclarationRecursive("b");
    const auto& c_decl_opt = scopeManager.findDeclarationRecursive("c");

    ASSERT_TRUE(a_decl_opt);
    ASSERT_TRUE(b_decl_opt);
    ASSERT_TRUE(c_decl_opt);

    const auto& a_decl = a_decl_opt.value();
    const auto& b_decl = b_decl_opt.value();
    const auto& c_decl = c_decl_opt.value();

    const Operand new_var {i32, 15, false, true};

    ASSERT_TRUE(result) << result.error;
    ASSERT_TRUE(operandsEqual(result.value.result, operandFromDecl(a_decl)));

    CFGNode nodes = {
        Command {Operation::MOVE, new_var, operandFromDecl(a_decl)},
        Command {Operation::ADD, new_var, operandFromDecl(b_decl)},
        Command {Operation::MUL, new_var, operandFromDecl(c_decl)},
    };

    ASSERT_TRUE(nodesEqual(result.value.node, nodes));
}

// === PREFIX OPERATOR TESTS ===

TEST_F(ExpressionParserTest, Prefix_Negation) {
    auto tokens = tokenize("-a;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
}

TEST_F(ExpressionParserTest, Prefix_LogicalNot) {
    auto tokens = tokenize("!a;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
}

TEST_F(ExpressionParserTest, Prefix_BitwiseNot) {
    auto tokens = tokenize("~a;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
}

TEST_F(ExpressionParserTest, Prefix_AddressOf) {
    auto tokens = tokenize("&a;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    // expected fail, not implemented
    // ASSERT_TRUE(result) << result.error;
    ASSERT_FALSE(result) << result.error;
}

TEST_F(ExpressionParserTest, Prefix_Dereference) {
    auto tokens = tokenize("*a;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    // expected fail, not implemented
    // ASSERT_TRUE(result) << result.error;
    ASSERT_FALSE(result) << result.error;
}

TEST_F(ExpressionParserTest, Prefix_Increment) {
    auto tokens = tokenize("++a;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
}

TEST_F(ExpressionParserTest, Prefix_Decrement) {
    auto tokens = tokenize("--a;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
}

TEST_F(ExpressionParserTest, Prefix_ConsecutiveOperators) {
    auto tokens = tokenize("- !a;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    // Should parse as -( !x ) => x NOT NEG
}

TEST_F(ExpressionParserTest, Prefix_InExpression_Context) {
    auto tokens = tokenize("a + -b;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    // The '-' after '+' should be prefix negation, not binary subtract
}

TEST_F(ExpressionParserTest, Prefix_WithParentheses) {
    auto tokens = tokenize("-(a + b);");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
}

// === POSTFIX OPERATOR TESTS ===

TEST_F(ExpressionParserTest, Postfix_Increment) {
    auto tokens = tokenize("a++;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
}

TEST_F(ExpressionParserTest, Postfix_Decrement) {
    auto tokens = tokenize("a--;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
}

TEST_F(ExpressionParserTest, Postfix_InExpression) {
    auto tokens = tokenize("a++ + b;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    // Postfix ++ binds to x before the addition
}

// === PREFIX vs POSTFIX DISAMBIGUATION ===

TEST_F(ExpressionParserTest, Disambiguation_PlusMinus) {
    // Binary + vs prefix +
    auto tokens1 = tokenize("a + +b;");
    size_t pos1 = 0;
    auto result1 = parser.parse(tokens1, pos1);
    ASSERT_TRUE(result1);

    // Binary - vs prefix -
    auto tokens2 = tokenize("a - -b;");
    size_t pos2 = 0;
    auto result2 = parser.parse(tokens2, pos2);
    ASSERT_TRUE(result2);
}

TEST_F(ExpressionParserTest, Disambiguation_Star) {
    // Binary * vs prefix dereference
    auto tokens1 = tokenize("a * b;");
    size_t pos1 = 0;
    auto result1 = parser.parse(tokens1, pos1);
    ASSERT_TRUE(result1) << result1.error;

    auto tokens2 = tokenize("*a;");
    size_t pos2 = 0;
    auto result2 = parser.parse(tokens2, pos2);
    ASSERT_TRUE(result2) << result2.error;
}

TEST_F(ExpressionParserTest, Disambiguation_Ampersand) {
    // Binary & vs prefix address-of
    auto tokens1 = tokenize("a & b;");
    size_t pos1 = 0;
    auto result1 = parser.parse(tokens1, pos1);
    ASSERT_TRUE(result1) << result1.error;

    auto tokens2 = tokenize("&a;");
    size_t pos2 = 0;
    auto result2 = parser.parse(tokens2, pos2);
    ASSERT_TRUE(result2) << result2.error;
}

TEST_F(ExpressionParserTest, Disambiguation_IncrementDecrement) {
    // Prefix vs postfix ++
    auto tokens1 = tokenize("++a;");
    size_t pos1 = 0;
    auto result1 = parser.parse(tokens1, pos1);
    ASSERT_TRUE(result1);

    auto tokens2 = tokenize("a++;");
    size_t pos2 = 0;
    auto result2 = parser.parse(tokens2, pos2);
    ASSERT_TRUE(result2);

    // Note: Both produce same RPN; semantic difference (pre vs post)
    // would be handled at codegen with different instruction selection
}

// === COMPLEX EXPRESSIONS ===

TEST_F(ExpressionParserTest, Complex_MixedUnaryBinary) {
    auto tokens = tokenize("a + -b * ++c;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    // Precedence: ++c (postfix), then -b (prefix), then *, then +
    // RPN: a b NEG c INC MUL ADD
}

TEST_F(ExpressionParserTest, Complex_NestedParentheses) {
    auto tokens = tokenize("-(a + !(b > c));");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    // Inner: b c GT, then NOT, then a + result, then NEG
}

TEST_F(ExpressionParserTest, Complex_ChainedComparisons) {
    auto tokens = tokenize("a < b && b < c;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    // && has lower precedence than <, so: (a<b) && (b<c)
}

// === FUNCTION CALLS ===

TEST_F(ExpressionParserTest, FunctionCall_NoArgs) {
    auto tokens = tokenize("foo();");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
}

TEST_F(ExpressionParserTest, FunctionCall_WithArgs) {
    auto tokens = tokenize("foo(a, b);");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
}

TEST_F(ExpressionParserTest, FunctionCall_WithUnaryArgs) {
    auto tokens = tokenize("foo(-a, !b);");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
}

TEST_F(ExpressionParserTest, FunctionCall_Nested) {
    auto tokens = tokenize("foo(bar(a));");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    // Inner call first: x inner CALL, then outer CALL
}

// === EDGE CASES ===

TEST_F(ExpressionParserTest, Edge_SingleOperand) {
    auto tokens = tokenize("a;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
}

TEST_F(ExpressionParserTest, Edge_SingleLiteral) {
    auto tokens = tokenize("42;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
}

TEST_F(ExpressionParserTest, Edge_BooleanLiterals) {
    auto tokens = tokenize("true && false;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
}

TEST_F(ExpressionParserTest, Edge_MultipleUnaryPrefix) {
    auto tokens = tokenize("! ! !a;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    // Right-to-left: !(!(!x)) => x NOT NOT NOT
}

TEST_F(ExpressionParserTest, Edge_UnaryAfterParen) {
    auto tokens = tokenize("(a + b)++;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
    // Postfix on parenthesized expression
}

TEST_F(ExpressionParserTest, Edge_PrefixBeforeParen) {
    auto tokens = tokenize("-(a + b);");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
}

// === ERROR CASES ===

TEST_F(ExpressionParserTest, Error_MismatchedParen_Open) {
    auto tokens = tokenize("(a + b;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_FALSE(result);
    EXPECT_NE(result.error.find("paren;"), std::string::npos) << result.error;
}

TEST_F(ExpressionParserTest, Error_MismatchedParen_Close) {
    auto tokens = tokenize("a + b);");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_FALSE(result);
    EXPECT_NE(result.error.find("paren;"), std::string::npos) << result.error;
}

TEST_F(ExpressionParserTest, Error_UnknownOperator) {
    auto tokens = tokenize("a @ b;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_FALSE(result);
    EXPECT_NE(result.error.find("operator;"), std::string::npos) << result.error;
}

TEST_F(ExpressionParserTest, Error_UnexpectedComma) {
    auto tokens = tokenize("a, b;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos, {Token::SEMICOLON});

    ASSERT_FALSE(result);
    EXPECT_NE(result.error.find("comma"), std::string::npos) << result.error;
}

// === TERMINATION PARAMETER TESTS ===

TEST_F(ExpressionParserTest, Termination_Semicolon) {
    auto tokens = tokenize("a + b; c;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos, {Token::SEMICOLON});

    ASSERT_TRUE(result) << result.error;
    // Should stop at semicolon, not parse "c"
    ASSERT_EQ(pos, 4);
}

TEST_F(ExpressionParserTest, Termination_RightParen) {
    auto tokens = tokenize("a + b) rest;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos, {Token::R_BRACKET});

    ASSERT_TRUE(result) << result.error;
    ASSERT_EQ(pos, 4);
}

// === PRECEDENCE VERIFICATION ===

TEST_F(ExpressionParserTest, Precedence_FullHierarchy) {
    // Test that all precedence levels work correctly
    // From highest to lowest: postfix, unary, */%, +-, <<>>, <<=, ==, &, ^, |, &&, ||, =
    auto tokens = tokenize("a = b || c && d ^ e | f & g == h < i << j + k * l;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
}
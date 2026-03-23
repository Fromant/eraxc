#include "frontend/syntax/parser/ExpressionParser.hpp"

#include <gtest/gtest.h>

#include "common/JIR/Function.hpp"
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
        scopeManager.addId("a", (size_t)type.value(), false, false);
        scopeManager.addId("b", (size_t)type.value(), false, false);
        scopeManager.addId("c", (size_t)type.value(), false, false);
        scopeManager.addId("d", (size_t)type.value(), false, false);
        scopeManager.addId("e", (size_t)type.value(), false, false);
        scopeManager.addId("f", (size_t)type.value(), false, false);
        scopeManager.addId("g", (size_t)type.value(), false, false);
        scopeManager.addId("h", (size_t)type.value(), false, false);
        scopeManager.addId("i", (size_t)type.value(), false, false);
        scopeManager.addId("j", (size_t)type.value(), false, false);
        scopeManager.addId("k", (size_t)type.value(), false, false);
        scopeManager.addId("l", (size_t)type.value(), false, false);

        auto fooId = scopeManager.addId("foo", (size_t)type.value(), true, false);
        if (fooId) {
            Function fooFunc;
            fooFunc.decl = Declaration(fooId.value(), jirTypeFromKeyword(Keyword::i32));
            scopeManager.getFunctions()[fooId.value()] = fooFunc;
        }

        auto barId = scopeManager.addId("bar", (size_t)type.value(), true, false);
        if (barId) {
            Function barFunc;
            barFunc.decl = Declaration(barId.value(), jirTypeFromKeyword(Keyword::i32));
            scopeManager.getFunctions()[barId.value()] = barFunc;
        }
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

    ASSERT_EQ(pos, 6);
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

    ASSERT_EQ(pos, 8);
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

    ASSERT_TRUE(result) << result.error;
}

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
    ASSERT_TRUE(result) << result.error;
}

TEST_F(ExpressionParserTest, Prefix_Dereference) {
    auto tokens = tokenize("*a;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);
    ASSERT_TRUE(result) << result.error;
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
}

TEST_F(ExpressionParserTest, Prefix_InExpression_Context) {
    auto tokens = tokenize("a + -b;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);
    ASSERT_TRUE(result) << result.error;
}

TEST_F(ExpressionParserTest, Prefix_WithParentheses) {
    auto tokens = tokenize("-(a + b);");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);
    ASSERT_TRUE(result) << result.error;
}

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
}

TEST_F(ExpressionParserTest, Disambiguation_PlusMinus) {
    auto tokens1 = tokenize("a + +b;");
    size_t pos1 = 0;
    auto result1 = parser.parse(tokens1, pos1);
    ASSERT_TRUE(result1) << result1.error;

    auto tokens2 = tokenize("a - -b;");
    size_t pos2 = 0;
    auto result2 = parser.parse(tokens2, pos2);
    ASSERT_TRUE(result2) << result2.error;
}

TEST_F(ExpressionParserTest, Disambiguation_Star) {
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
    auto tokens1 = tokenize("++a;");
    size_t pos1 = 0;
    auto result1 = parser.parse(tokens1, pos1);
    ASSERT_TRUE(result1);

    auto tokens2 = tokenize("a++;");
    size_t pos2 = 0;
    auto result2 = parser.parse(tokens2, pos2);
    ASSERT_TRUE(result2);
}

TEST_F(ExpressionParserTest, Complex_MixedUnaryBinary) {
    auto tokens = tokenize("a + -b * ++c;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);
    ASSERT_TRUE(result) << result.error;
}

TEST_F(ExpressionParserTest, Complex_NestedParentheses) {
    auto tokens = tokenize("-(a + !(b > c));");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);
    ASSERT_TRUE(result) << result.error;
}

TEST_F(ExpressionParserTest, Complex_ChainedComparisons) {
    auto tokens = tokenize("a < b && b < c;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);
    ASSERT_TRUE(result) << result.error;
}

TEST_F(ExpressionParserTest, FunctionCall_NoArgs) {
    auto tokens = tokenize("foo();");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);
    ASSERT_TRUE(result) << result.error;
}

TEST_F(ExpressionParserTest, FunctionCall_WithArgs) {
    auto tokens = tokenize("bar(a, b);");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);
    ASSERT_FALSE(result) << result.error;
}

TEST_F(ExpressionParserTest, FunctionCall_WithUnaryArgs) {
    auto tokens = tokenize("bar(-a, !b);");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);
    ASSERT_FALSE(result) << result.error;
}

TEST_F(ExpressionParserTest, FunctionCall_Nested) {
    auto tokens = tokenize("bar(a, foo());");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);
    ASSERT_FALSE(result) << result.error;
}

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
    // TODO not impl
    ASSERT_FALSE(result) << result.error;
}

TEST_F(ExpressionParserTest, Edge_MultipleUnaryPrefix) {
    auto tokens = tokenize("! ! !a;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);
    ASSERT_TRUE(result) << result.error;
}

TEST_F(ExpressionParserTest, Edge_UnaryAfterParen) {
    auto tokens = tokenize("(a + b)++;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);
    ASSERT_TRUE(result) << result.error;
}

TEST_F(ExpressionParserTest, Edge_PrefixBeforeParen) {
    auto tokens = tokenize("-(a + b);");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);
    ASSERT_TRUE(result) << result.error;
}

TEST_F(ExpressionParserTest, Error_MismatchedParen_Open) {
    auto tokens = tokenize("(a + b;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_FALSE(result);
    EXPECT_NE(result.error.find(";"), std::string::npos) << result.error;
}

TEST_F(ExpressionParserTest, Error_MismatchedParen_Close) {
    auto tokens = tokenize("a + b);");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_FALSE(result);
    EXPECT_NE(result.error.find(")"), std::string::npos) << result.error;
}

TEST_F(ExpressionParserTest, Error_UnknownOperator) {
    auto tokens = tokenize("a @ b;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_FALSE(result);
    EXPECT_NE(result.error.find("@"), std::string::npos) << result.error;
}

TEST_F(ExpressionParserTest, Error_UnexpectedComma) {
    auto tokens = tokenize("a, b;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos, {Token::SEMICOLON});

    ASSERT_FALSE(result);
    EXPECT_NE(result.error.find(","), std::string::npos) << result.error;
}

TEST_F(ExpressionParserTest, Termination_Semicolon) {
    auto tokens = tokenize("a + b; c;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos, {Token::SEMICOLON});

    ASSERT_TRUE(result) << result.error;
    ASSERT_EQ(pos, 4);
}

TEST_F(ExpressionParserTest, Termination_RightParen) {
    auto tokens = tokenize("a + b) rest;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos, {Token::R_BRACKET});

    ASSERT_TRUE(result) << result.error;
    ASSERT_EQ(pos, 4);
}

TEST_F(ExpressionParserTest, Precedence_FullHierarchy) {
    auto tokens = tokenize("a = b || c && d ^ e | f & g == h < i << j + k * l;");
    size_t pos = 0;
    auto result = parser.parse(tokens, pos);

    ASSERT_TRUE(result) << result.error;
}

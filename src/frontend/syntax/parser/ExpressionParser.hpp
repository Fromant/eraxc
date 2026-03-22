#pragma once

#include "common/CFG/CFGParts.hpp"
#include "common/JIR/Function.hpp"
#include "common/JIR/JIR.hpp"
#include "frontend/lexic/PreprocessorTokenizer.hpp"
#include "frontend/syntax/scope/ScopeManager.hpp"
#include "util/error.hpp"

namespace eraxc::frontend {

    class ExpressionParser {

        ScopeManager& scope_manager;
        JIR::FrontendResult& result;

    public:
        explicit ExpressionParser(ScopeManager& scope_manager, JIR::FrontendResult& result) :
            scope_manager(scope_manager), result(result) {};

        struct ParseResult {
            CFG::CFGNode node;
            // result of expr
            JIR::Operand result;
        };

        error::errable<ParseResult> parse(const std::vector<Token>& tokens, size_t& pos,
                                          const std::set<Token::type>& end = {Token::SEMICOLON});
        error::errable<void> push_expr_stack(std::stack<OperatorType>& operations, std::stack<JIR::Operand>& operands,
                                             std::vector<JIR::Command>& cmds) const;
        error::errable<JIR::Operand> parse_expr_operand(const std::vector<Token>& tokens, size_t& pos,
                                                        std::vector<JIR::Command>& cmds,
                                                        std::vector<JIR::Command>& postfix_cmds);

        struct ParseCondResult {
            CFG::CFGNode node;
            JIR::Operation jump_op;
        };

        error::errable<ParseCondResult> parse_cond(const std::vector<Token>& tokens, size_t& pos,
                                                   const std::set<Token::type>& end = {Token::L_BRACKET});
        error::errable<JIR::Operation> push_cond_expr_stack(std::stack<OperatorType>& operations,
                                                            std::stack<JIR::Operand>& operands,
                                                            std::vector<JIR::Command>& cmds) const;
    };

}

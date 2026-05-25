#pragma once

#include "common/CFG/CFGParts.hpp"
#include "common/JIR/Command.hpp"
#include "common/JIR/FrontendResult.hpp"
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
                                             CFG::CFGNode& node) const;
        error::errable<JIR::Operand> parse_expr_operand(const std::vector<Token>& tokens, size_t& pos,
                                                        CFG::CFGNode& node, std::vector<JIR::Command>& postfix_cmds);

        struct ParseCondResult {
            CFG::CFGNode node;
            JIR::Command jump_cmd;
        };
    };

}

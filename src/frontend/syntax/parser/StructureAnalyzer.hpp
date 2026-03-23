#pragma once
#include "ExpressionParser.hpp"
#include "common/CFG/CFGParts.hpp"
#include "common/JIR/Function.hpp"
#include "frontend/lexic/PreprocessorTokenizer.hpp"
#include "util/error.hpp"

namespace eraxc::frontend {

    class StructureAnalyzer {

        ScopeManager scopeManager;
        JIR::FrontendResult result;

        ExpressionParser parser {scopeManager, result};


    public:
        StructureAnalyzer() = default;
        error::errable<JIR::Function> parseFunction(const std::vector<Token>& tokens, size_t& pos);
        error::errable<void> parseStatements(const std::vector<Token>& tokens, size_t& pos, CFG::CFG& cfg,
                                             size_t& node_id);
        error::errable<void> parseStatement(const std::vector<Token>& tokens, size_t& pos, CFG::CFG& cfg,
                                            size_t& node_id);
        error::errable<void> parseIf(const std::vector<Token>& tokens, size_t& pos, CFG::CFG& cfg, size_t& node_id);
        error::errable<void> parseWhile(const std::vector<Token>& tokens, size_t& pos, CFG::CFG& cfg, size_t& node_id);
        error::errable<void> parseDeclaration(const std::vector<Token>& tokens, size_t& pos, CFG::CFG& cfg,
                                              const size_t& node_id);
        error::errable<JIR::FrontendResult> analyze(const std::vector<Token>& tokens);
    };

}

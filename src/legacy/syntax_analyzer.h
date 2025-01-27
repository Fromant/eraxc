#ifndef ERAXC_SYNTAX_ANALYZER_H
#define ERAXC_SYNTAX_ANALYZER_H


#include <set>
#include <string>
#include <utility>
#include <iostream>

#include "lexic.h"
#include "../frontend/syntax/enums.h"
#include "AST.h"


namespace eraxc::syntax {

    static inline std::unordered_map<std::string, size_t> string_instants{};

    struct syntax_analyzer {
        scope global_scope{nullptr};

        const std::string &get_typename(size_t id) const;
        const std::string &get_identifier(size_t id) const;

        error::errable<std::vector<AST::node>> analyze(const std::vector<lexic::Token> &ts);

        static error::errable<AST::expr_node *>
        parse_expr_node(const std::vector<lexic::Token> &ts, size_t &i, scope &scope);

        static error::errable<AST::expr_node *>
        parse_expr(const std::vector<lexic::Token> &ts, size_t &i, scope &scope, lexic::Token::type end);

        static error::errable<std::vector<AST::statement_node>>
        parse_func_body(const std::vector<lexic::Token> &ts, size_t &i, scope &scope);

        error::errable<AST::func_node *>
        parse_func(const std::vector<lexic::Token> &ts, size_t &i, scope &scope) const;

        static error::errable<void>
        parse_function_definition(const std::vector<lexic::Token> &ts, size_t &i, scope &global_scope);

        static error::errable<AST::decl_node> parse_decl(const std::vector<lexic::Token> &ts,
                                                         size_t &i,
                                                         scope &scope,
                                                         lexic::Token::type);
    };
}

#endif //ERAXC_SYNTAX_ANALYZER_H

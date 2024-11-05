#include "syntax_analyzer.h"

namespace blck::syntax {

    const std::string &get_typename(size_t id) {
        for (const auto &it: global_scope.typenames) {
            if (it.second == id) return it.first;
        }
        return NOT_FOUND;
    }

    const std::string &get_identifier(size_t id) {
        for (const auto &it: global_scope.identifiers) {
            if (it.second == id) return it.first;
        }
        return NOT_FOUND;
    }

    void AST::node::print() const {
        if (type == DECLARATION) {
            d.decl.print();
        } else if (type == FUNCTION) {
            d.func->print();
        }
    }

    error::errable<AST::expr_node *> syntax_analyzer::parse_expr_node(const lexic::Token &t, scope &scope) {
        size_t data;
        if (t.t == lexic::Token::INSTANT) {
            data = std::stoll(t.data);
            return {"", new AST::expr_node{ASSIGN, data, true}};
        } else if (t.t == lexic::Token::STRING_INSTANT) {
            data = string_instants.size();
            auto r = string_instants.emplace(t.data, data);
            if (!r.second) {
                data = r.first->second;
            }
            return {"", new AST::expr_node{ASSIGN, data, true}};
        } else if (t.t == lexic::Token::IDENTIFIER) {
            if (scope.contains_id(t.data)) {
                data = scope.get_id(t.data);
                return {"", new AST::expr_node{ASSIGN, data, false}};
            } else return {"UNKNOWN IDENTIFIER: " + t.data, new AST::expr_node{NONE, 0xFFFFFFFF, false}};
        } else
            return {"EXPECTED IDENTIFIER OR INSTANT", new AST::expr_node{NONE, 0xFFFFFFFF, false}};
    }

    error::errable<AST::expr_node *>
    syntax_analyzer::parse_expr(const std::vector<lexic::Token> &ts, int &i, scope &scope) {
        //start with first operand
        //TODO resolve unary operations
        auto node = parse_expr_node(ts[i], scope);
        if (!node) return node;
        auto *cur = node.value;
        auto top = cur;
        bool isFull = false;
        i++;

        lexic::Token t = ts[i];
        while (t.t != lexic::Token::SEMICOLON) {
            if (isFull) {
                node = parse_expr_node(ts[i], scope);
                if (!node) return node;
                cur->right = node.value;
                cur = cur->right;
            } else {
                if (ts[i].t != lexic::Token::OPERATOR)
                    return {"EXPECTED OPERATOR", new AST::expr_node{NONE, 0xFFFFFFFF, false}};
                cur->op = blck::syntax::operators.at(ts[i].data);
            }
            isFull = !isFull;
            i++;
            t = ts[i];
        }

        return {"", top};
    }

    error::errable<std::vector<AST::statement_node>>
    parse_func_body(const std::vector<lexic::Token> &ts, int &i, scope &scope) {
        std::vector<AST::statement_node> tr{};

        while (ts[i].t != lexic::Token::R_F_BRACKET) {
            AST::statement_node ta{};

            //TODO parse if for while return

            if (ts[i].t == lexic::Token::IDENTIFIER && ts[i + 1].t == lexic::Token::IDENTIFIER) {
                if (ts[i + 2].t == lexic::Token::L_BRACKET) {
                    return {"CANNOT DECLARE A FUNCTION INSIDE A FUNCTION", tr};
                } else {
                    auto decl = syntax_analyzer::parse_decl(ts, i, scope, lexic::Token::SEMICOLON);
                    if (!decl) return {decl.error, tr};
                    ta.type = AST::statement_node::DECLARATION;
                    ta.data.decl = decl.value;
                    tr.emplace_back(ta);
                }
            } else {
                auto expr = syntax_analyzer::parse_expr(ts, i, scope);
                if (!expr) return {expr.error, tr};
                ta.type = AST::statement_node::EXPRESSION;
                ta.data.expr = expr.value;
                tr.emplace_back(ta);
            }
            i++;
            if (i == ts.size()) return {"EXPECTED END OF FUNCTION BEFORE EOF", tr};
        }

        return {"", tr};
    }

    error::errable<AST::func_node *> syntax_analyzer::parse_func(const std::vector<lexic::Token> &ts,
                                                                 int &i,
                                                                 scope &global_scope) {
        scope func_scope{&global_scope};
        auto *tr = new AST::func_node();

        auto type = func_scope.get_type(ts[i].data);
        if (type == -1) {
            return {"NO SUCH TYPENAME IN THIS SCOPE: " + ts[i].data, tr};
        }
        if (func_scope.contains_id(ts[i + 1].data)) {
            return {ts[i + 1].data + " IS ALREADY DEFINED IN THIS SCOPE", tr};
        }
        auto id = global_scope.add_id(ts[i + 1].data);

        tr->return_typename = type;
        tr->id = id;

        i += 3;
        //parse args decl
        while (ts[i].t != lexic::Token::R_BRACKET) {
            auto t = parse_decl(ts, i, func_scope, lexic::Token::R_BRACKET);
            while (ts[i].t == lexic::Token::COMMA && t) {
                i++;
                tr->args.push_back(t.value);
                t = parse_decl(ts, i, func_scope, lexic::Token::R_BRACKET);
            }
            if (!t) return {t.error, tr};
            tr->args.push_back(t.value);
            if (i == ts.size()) return {"EXPECTED END OF FUNCTION ARGUMENTS DECLARATION BEFORE EOF", tr};
        }
        i++;

        //parse function body
        if (ts[i].t != lexic::Token::L_F_BRACKET) {
            return {"EXPECTED FUNCTION BODY '{...}': " + ts[i].data, tr};
        }
        i++;

        auto body = parse_func_body(ts, i, func_scope);
        if (!body) return {body.error, tr};
        tr->body = body.value;

        return {"", tr};
    }

    error::errable<AST::decl_node> syntax_analyzer::parse_decl(const std::vector<lexic::Token> &ts,
                                                               int &i,
                                                               scope &scope,
                                                               lexic::Token::type end = lexic::Token::SEMICOLON) {
        AST::decl_node tr{};
        if (!scope.contains_type(ts[i].data)) {
            return {"NO SUCH TYPENAME: " + ts[i].data, tr};
        }
        if (scope.cur_contains_id(ts[i + 1].data)) {
            return {ts[i + 1].data + " IS ALREADY DEFINED", tr};
        }

        tr.type = scope.get_type(ts[i].data);
        tr.id = scope.add_id(ts[i + 1].data);

        if (ts[i + 2].t == lexic::Token::OPERATOR && ts[i + 2].data == "=") {
            i += 3;
            auto assign = parse_expr(ts, i, scope);

            if (!assign) return {assign.error, tr};

            tr.assign_to = *assign.value;

            return {"", tr};
        } else if (ts[i + 2].t == lexic::Token::COMMA) {
            i += 2;
            return {"", tr};
        } else if (ts[i + 2].t != end) {
            return {"EXPECTED OPERATOR= or '" + std::to_string(end) + "': " + ts[i].data, tr};
        }
        i += 2;
        return {"", tr};
    }

    error::errable<std::vector<AST::node>> syntax_analyzer::analyze(const std::vector<lexic::Token> &ts) {
        using namespace lexic;

        std::vector<AST::node> tr{};
        int i = 0;
        while (ts[i].t != Token::NONE) {
            if (ts[i].t == Token::IDENTIFIER && ts[i + 1].t == Token::IDENTIFIER) {
                if (ts[i + 2].t == Token::L_BRACKET) {
                    //parse func
                    AST::node ta{AST::node::FUNCTION};
                    auto func = parse_func(ts, i, global_scope);
                    if (!func) return {func.error, tr};
                    ta.d.func = func.value;
                    tr.emplace_back(ta);
                } else {
                    //parse decl
                    AST::node ta{AST::node::DECLARATION};
                    auto decl = parse_decl(ts, i, global_scope);
                    if (!decl) return {decl.error, tr};
                    ta.d.decl = decl.value;
                    tr.emplace_back(ta);
                }
            }
            i++;
        }
        return {std::string{""}, tr};
    }
}
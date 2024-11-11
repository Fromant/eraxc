#include "syntax_analyzer.h"

namespace eraxc::syntax {

    const std::string &syntax_analyzer::get_typename(size_t id) {
        for (const auto &it: global_scope.typenames) {
            if (it.second == id) return it.first;
        }
        return NOT_FOUND;
    }

    const std::string &syntax_analyzer::get_identifier(size_t id) {
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

    error::errable<AST::expr_node *>
    syntax_analyzer::parse_expr_node(const std::vector<lexic::Token> &ts, size_t &i, scope &scope) {
        auto t = ts[i];
        auto *tr = new AST::expr_node();
        tr->op = NONE;
        if (t.t == lexic::Token::OPERATOR) {
            auto it = unary_operators.find(t.data);
            if (it != unary_operators.cend()) {
                tr->unary = it->second;
                i++;
            } else return {t.data + " IS NOT AN UNARY OPERATOR. UNARY EXPECTED", tr};
        }
        t = ts[i];
        if (t.t == lexic::Token::L_BRACKET) {
            i++;
            auto r = parse_expr(ts, i, scope, lexic::Token::R_BRACKET);
            if (!r) return {r.error, tr};
            tr->hasParenthesis = true;
            tr->data.parenthesis = r.value;
        } else if (t.t == lexic::Token::INSTANT) {
            tr->isInstant = true;
            tr->data.data = std::stoll(t.data);
        } else if (t.t == lexic::Token::STRING_INSTANT) {
            tr->isInstant = true;
            tr->data.data = string_instants.size();
            auto r = string_instants.emplace(t.data, tr->data.data);
            if (!r.second) tr->data.data = r.first->second;
        } else if (t.t == lexic::Token::IDENTIFIER) {
            tr->isInstant = false;
            auto id = scope.get_id(t.data);
            if (id != -1) {
                tr->data.data = id;
            } else return {"UNKNOWN IDENTIFIER: " + t.data, new AST::expr_node{NONE, 0xFFFFFFFF, false}};
        } else
            return {"EXPECTED IDENTIFIER OR INSTANT OR '('", new AST::expr_node{NONE, 0xFFFFFFFF, false}};


        i++;
        auto it = postfix_operators.find(t.data);
        if (t.t == lexic::Token::OPERATOR && it != postfix_operators.end()) {
            //there is a postfix operator
            tr->postfix = it->second;
            i++;
        }

        return {"", tr};
    }

    error::errable<AST::expr_node *>
    syntax_analyzer::parse_expr(const std::vector<lexic::Token> &ts, size_t &i, scope &scope,
                                lexic::Token::type end = lexic::Token::SEMICOLON) {
        //start with first operand
        auto node = parse_expr_node(ts, i, scope);
        if (!node) return node;
        auto *cur = node.value;
        auto top = cur;
        bool isFull = false;

        lexic::Token t = ts[i];
        while (t.t != end) {
            if (isFull) {
                node = parse_expr_node(ts, i, scope);
                if (!node) return node;
                cur->right = node.value;
                cur = cur->right;
            } else {
                if (ts[i].t != lexic::Token::OPERATOR)
                    return {"EXPECTED OPERATOR",
                            new AST::expr_node{NONE, 0xFFFFFFFF, false}};
                cur->op = eraxc::syntax::operators.at(ts[i].data);
                i++;
            }
            isFull = !isFull;
            t = ts[i];
        }

        return {"", top};
    }

    error::errable<std::vector<AST::statement_node>>
    syntax_analyzer::parse_func_body(const std::vector<lexic::Token> &ts, size_t &i,
                                     eraxc::syntax::scope &scope) {
        std::vector<AST::statement_node> tr{};

        while (ts[i].t != lexic::Token::R_F_BRACKET) {
            AST::statement_node ta{};

            //TODO parse if for while

            if (ts[i].t == lexic::Token::IDENTIFIER && ts[i].data == "return") {
                i++;
                ta.type = AST::statement_node::RETURN;
                auto expr = parse_expr(ts, i, scope);
                i++;
                if (!expr) return {expr.error, tr};
                ta.data.ret = expr.value;
                tr.emplace_back(ta);
                //gen error if there is more statements after return
                //TODO nah that's all shi because of branching and anonymous namespaces
                if (ts[i].t != lexic::Token::R_F_BRACKET)
                    return {"EXPECTED END OF FUNCTION BODY '}' AFTER RETURN: " + ts[i].data, tr};
                break;
            }

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
                                                                 size_t &i,
                                                                 scope &global_scope) {
        scope func_scope{&global_scope};
        auto *tr = new AST::func_node();

        auto type = func_scope.get_type(ts[i].data);
        if (type == -1) {
            return {"NO SUCH TYPENAME IN THIS SCOPE: " + ts[i].data, tr};
        }
        auto id = global_scope.get_id(ts[i + 1].data);

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

        if (ts[i].t != lexic::Token::L_F_BRACKET) {
            return {"EXPECTED FUNCTION BODY '{...}': " + ts[i].data, tr};
        }
        i++;

        //parse function body
        auto body = parse_func_body(ts, i, func_scope);
        if (!body) return {body.error, tr};
        tr->body = body.value;

        //check if last is return
        if (tr->return_typename != VOID && tr->body[tr->body.size() - 1].type != AST::statement_node::RETURN) {
            return {"EXPECTED RETURN IN A FUNCTION RETURNING NON-VOID: " + get_identifier(tr->id), tr};
        }

        return {"", tr};
    }

    error::errable<AST::decl_node> syntax_analyzer::parse_decl(const std::vector<lexic::Token> &ts,
                                                               size_t &i,
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


/// function to parse function definition with skipping all the declaration
/// \param ts vector of tokens
/// \param i current index
/// \param global_scope current global scope
/// \return index of last token of function
    error::errable<void>
    syntax_analyzer::parse_function_definition(const std::vector<lexic::Token> &ts, size_t &i, scope &global_scope) {
        auto type = global_scope.get_type(ts[i].data);
        if (type == -1) {
            return {"NO SUCH TYPENAME IN THIS SCOPE: " + ts[i].data};
        }
        if (global_scope.contains_id(ts[i + 1].data)) {
            return {ts[i + 1].data + " IS ALREADY DEFINED IN THIS SCOPE"};
        }
        global_scope.add_id(ts[i + 1].data);

        i += 3;
        //skip args decl
        while (ts[i].t != lexic::Token::R_BRACKET) {
            i++;
            if (i == ts.size()) return {"EXPECTED END OF FUNCTION ARGUMENTS DECLARATION BEFORE EOF"};
        }
        i++;

        if (ts[i].t != lexic::Token::L_F_BRACKET) {
            return {"EXPECTED FUNCTION BODY '{...}': " + ts[i].data};
        }
        i++;

        //skip function body
        while (ts[i].t != lexic::Token::R_F_BRACKET) {
            i++;
            if (i == ts.size()) return {"EXPECTED END OF FUNCTION BODY BEFORE EOF"};
        }

        return {""};
    }

    error::errable<std::vector<AST::node>> syntax_analyzer::analyze(const std::vector<lexic::Token> &ts) {
        using namespace lexic;

        std::vector<AST::node> tr{};
        size_t i = 0;

        //TODO 0 iter - parse all types declaration (enums & structs & typedefs)

        //first iter - parse all definition(including functions)
        //and save all functions, so we can loop over them and parse declarations later
        std::vector<size_t> fns{};
        while (ts[i].t != Token::NONE) {
            if (ts[i].t == Token::IDENTIFIER && ts[i + 1].t == Token::IDENTIFIER) {
                if (ts[i + 2].t == Token::L_BRACKET) {
                    //parse func definition
                    size_t start = i;
                    auto t = parse_function_definition(ts, i, global_scope);
                    if (!t) return {t.error, tr};
                    fns.emplace_back(start);
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


        //iter 2 - parse all functions declaration
        i = 0;

        for (auto &start: fns) {
            i = start;
            scope func_scope{global_scope};
            auto t = parse_func(ts, i, func_scope);
            if (!t) return {t.error, tr};
            AST::node ta{AST::node::FUNCTION};
            ta.d.func = t.value;
            tr.emplace_back(ta);
        }

        while (ts[i].t != Token::NONE) {
            if (ts[i].t == Token::IDENTIFIER && ts[i + 1].t == Token::IDENTIFIER && ts[i + 2].t == Token::L_BRACKET) {
                //parse func
                AST::node ta{AST::node::FUNCTION};
                auto func = parse_func(ts, i, global_scope);
                if (!func) return {func.error, tr};
                ta.d.func = func.value;
                tr.emplace_back(ta);
            }
            i++;
        }
        return {std::string{""}, tr};
    }

}
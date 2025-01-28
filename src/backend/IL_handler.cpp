#include "IL_handler.h"

namespace eraxc::IL {

    error::errable<std::vector<IL_node>> IL_handler::translate_assignment(const std::vector<token> &tokens,
                                                                          int &i, const scope &scope) {
        std::vector<IL_node> tr{};

        scope::identifier changed_id = scope.get_id(tokens[i].data);

        syntax::operator_type assignment_type = syntax::operators.at(tokens[i + 1].data);

        if(tokens[i + 2].t != token::IDENTIFIER && tokens[i + 2].t != token::INSTANT)
            return {"Expected identifier or instant instead of " + tokens[i + 1].data, tr};
        u64 operand1 = tokens[i + 2].t == token::INSTANT ? std::stoull(tokens[i + 2].data) :
                       scope.get_id(tokens[i + 2].data).id;
        bool isop1instant = tokens[i + 2].t == token::INSTANT;

        i += 3;

        while(tokens[i].t != token::SEMICOLON) {
            if(tokens[i].t == token::NONE) return {"Expected end of expression before EOF", tr};

            if(tokens[i].t != token::OPERATOR) return {"Expected operator instead of " + tokens[i].data, tr};
            syntax::operator_type op = syntax::operators.at(tokens[i].data);
            if(tokens[i + 1].t != token::IDENTIFIER && tokens[i + 1].t != token::INSTANT)
                return {"Expected identifier or instant instead of " + tokens[i + 1].data, tr};
            u64 next_operand = tokens[i + 1].t == token::INSTANT ? std::stoull(tokens[i + 1].data) :
                               scope.get_id(tokens[i + 1].data).id;

            tr.emplace_back(operand1, next_operand, op, isop1instant, true);

            i += 2;
        }

        return {"", tr};
    }

    error::errable<std::vector<IL_node>> IL_handler::parse_declaration(const std::vector<token> &tokens,
                                                                       int &i, scope &scope) {
        if(!scope.contains_type(tokens[i].data)) return {"No such typename " + tokens[i].data, {}};
        const u64 type = scope.get_type_id(tokens[i].data);

        if(scope.cur_contains_id(tokens[i + 1].data))
            return {"Variable " + tokens[i + 1].data + " is already defined in this scope", {}};

        scope.add_id(tokens[i + 1].data, type, false);

        if(tokens[i + 2].t == token::SEMICOLON) {
            i += 2;
            return {"", {}};
        }; //end of declaration
        if(tokens[i + 2].t != token::OPERATOR || tokens[i + 2].data != "=")
            return {"Expected operator= or ';' at the end of declaration", {}};
        i++; //now tokens[i] is a name of variable
        //parse expr
        return translate_assignment(tokens, i, scope);
    }

    //translates all statemtnts till '}'
    error::errable<std::vector<IL_node>> IL_handler::translate_statements(const std::vector<token> &tokens,
                                                                          int &i, scope &scope) {
        std::vector<IL_node> tr;

        while(tokens[i].t != token::NONE && tokens[i].t != token::R_F_BRACKET) {
            if(tokens[i].t == token::IDENTIFIER) {
                if(tokens[i].data == "return") {
                    //TODO return statement
                    return {"", tr};
                } else if(tokens[i + 1].t == token::IDENTIFIER) {
                    //decl
                    auto init = parse_declaration(tokens, i, scope);
                    if(!init) return {init.error, tr};
                    tr.insert(tr.cend(), init.value.cbegin(), init.value.cend());
                } else if(tokens[i + 1].t == token::OPERATOR) {
                    //expr
                    auto expr = translate_assignment(tokens, i, scope);
                    if(!expr) return {expr.error, tr};
                    tr.insert(tr.cend(), expr.value.cbegin(), expr.value.cend());
                } else if(tokens[i + 1].t == token::L_BRACKET) {
                    //if, for, while, switch(?)
                }
            }
            if(tokens[i].t != token::R_F_BRACKET)
                return {"Expected end of function body '}' before the EOF", tr};
        }
        return {"", tr};
    }

    error::errable<void> IL_handler::translate_function(const std::vector<token> &tokens, int &i) {
        if(!global_scope.contains_type(tokens[i].data)) return {"No such typename " + tokens[i].data};
        const u64 return_type = global_scope.get_type_id(tokens[i].data);

        if(global_scope.cur_contains_id(tokens[i + 1].data))
            return {"Variable " + tokens[i + 1].data + " is already defined in this scope"};

        const u64 func_id = global_scope.add_id(tokens[i + 1].data, return_type, true);

        scope func_scope{global_scope};
        std::vector<IL_node> instructions;
        std::vector<IL_decl> args;

        i += 3;

        //parse arguments declaration
        while(tokens[i].t != token::R_BRACKET) {
            if(tokens[i].t == token::NONE) return {"Unexpected EOF in arguements"};
            if(tokens[i].t == token::IDENTIFIER) {
                if(!func_scope.contains_type(tokens[i].data)) return {"No such typename " + tokens[i].data};
                if(tokens[i + 1].t != token::IDENTIFIER)
                    return {"Expected variable name in arguments list instead of " + tokens[i + 1].data};
                func_scope.add_id(tokens[i + 1].data, func_scope.get_type_id(tokens[i].data), false);
            } else
                return {"Expected function variable list or end of function devlaration instead of " + tokens[i].data};
            if(!(tokens[i + 2].t == token::COMMA || tokens[i + 2].t == token::R_BRACKET))
                return {"Expected comma ',' or right bracket instead of " + tokens[i + 2].data};
            i += 3;
        }

        //parse funciton body
        if(tokens[i].t != token::L_F_BRACKET)
            return {"Expected function body instead of " + tokens[i].data};
        i++;
        auto r = translate_statements(tokens, i, func_scope);
        if(!r) return {r.error};

        global_funcs[func_id] = {IL_decl{func_id, return_type, true}, args, r.value};

        return {""};
    }


    error::errable<void> IL_handler::translate(const std::vector<token> &tokens) {
        int i = 0;
        while(i < tokens.size()) {
            if(tokens[i].t == token::IDENTIFIER && tokens[i + 1].t == token::IDENTIFIER) {
                // if(tokens[i].data == "namespace") {//namespaces support}
                if(tokens[i + 2].t == token::L_BRACKET) {
                    translate_function(tokens, i);
                } else {
                    auto r = parse_declaration(tokens, i, global_scope);
                    if(!r) return {r.error};
                    global_init.insert(global_init.cend(), r.value.cbegin(), r.value.cend());
                }
                i++;
            }
        }

        return {""};
    }
}

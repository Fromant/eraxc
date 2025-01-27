#include "IL_handler.h"

namespace eraxc::IL {

    error::errable<std::vector<IL_node>> IL_handler::translate_expr(const std::vector<token>& tokens, int& i,
                                                                    const std::unordered_map<std::string, IL_decl>&
                                                                    scope) {
        std::vector<IL_node> tr{};


        return {"", tr};
    }


    error::errable<std::vector<IL_node>> IL_handler::parse_declaration(const std::vector<token>& tokens, int& i,
                                                                       std::unordered_map<std::string, IL_decl>&
                                                                       scope) {
        std::vector<IL_node> tr;
        if (!typenames.contains(tokens[i].data)) return {"No such typename " + tokens[i].data, tr};
        const u64 type = typenames[tokens[i].data];
        if (scope.contains(tokens[i + 1].data))
            return {"Variable " + tokens[i + 1].data + " is already defined in this scope", tr};
        scope[tokens[i + 1].data] = IL_decl{scope.size(), type, false};

        if (tokens[i + 2].t == token::SEMICOLON) return {"", tr}; //end of declaration
        if (tokens[i + 2].t != token::OPERATOR || tokens[i + 2].data != "=")
            return {
                "Expected operator= or ';' at the end of declaration", tr
            };
        i++; //now tokens[i] is a name of variiable
        //parse expr
        auto r = translate_expr(tokens, i, scope);
        if (!r) return {r.error, tr};
        tr.insert(tr.cend(), r.value.cbegin(), r.value.cend());
        return {"", tr};
    }

    //translates all statemtnts till '}'
    error::errable<std::vector<IL_node>> IL_handler::translate_statements(
        const std::vector<token>& tokens, int& i,
        std::unordered_map<std::string, IL_decl>& local_vars) {
        std::vector<IL_node> tr;

        while (tokens[i].t != token::NONE && tokens[i].t != token::R_F_BRACKET) {
            if (tokens[i].t == token::IDENTIFIER) {
                if (tokens[i].data == "return") {
                    //TODO return statement
                    return {"", tr};
                } else if (tokens[i + 1].t == token::IDENTIFIER) {
                    //decl
                    auto init = parse_declaration(tokens, i, local_vars);
                    if (!init) return {init.error, tr};
                    tr.insert(tr.cend(), init.value.cbegin(), init.value.cend());
                } else if (tokens[i + 1].t == token::OPERATOR) {
                    //expr
                    auto expr = translate_expr(tokens,i,local_vars);
                    if(!expr) return {expr.error, tr};
                    tr.insert(tr.cend(), expr.value.cbegin(), expr.value.cend());
                } else if (tokens[i + 1].t == token::L_BRACKET) {
                    //if, for, while, switch(?)
                }
            }
            if (tokens[i].t != token::R_F_BRACKET)
                return {"Expected end of function body '}' before the EOF", tr};
        }
        return {"", tr};
    }

    error::errable<void> IL_handler::translate_function(const std::vector<token>& tokens, int& i) {
        if (!typenames.contains(tokens[i].data)) return {"No such typename " + tokens[i].data};
        const u64 return_type = typenames[tokens[i].data];

        if (global_vars.contains(tokens[i + 1].data))
            return {"Variable " + tokens[i + 1].data + " is already defined in this scope"};

        const u64 func_id = global_vars.size();

        global_vars.emplace(tokens[i].data, IL_decl{func_id, return_type, true});

        std::unordered_map<std::string, IL_decl> local_vars{};
        std::vector<IL_node> instructions;

        std::vector<IL_decl> args;

        i += 3;

        //parse arguments declaration
        while (tokens[i].t != token::R_BRACKET) {
            if(tokens[i].t == token::NONE) return {"Unexpected EOF in arguements"};
            if (tokens[i].t == token::IDENTIFIER) {
                if (!typenames.contains(tokens[i].data)) return {"No such typename " + tokens[i].data};
                if (tokens[i + 1].t != token::IDENTIFIER)
                    return {
                        "Expected variable name in arguments list instead of " + tokens[i + 1].data
                    };
                local_vars[tokens[i + 1].data] = {
                    func_id + local_vars.size() + 1, typenames[tokens[i + 3].data], false
                };
                args.emplace_back(func_id + local_vars.size() + 1, typenames[tokens[i + 3].data], false);
            } else
                return {
                    "Expected function variable list or end of function devlaration instead of " + tokens[i].data
                };
            if (tokens[i + 2].t != token::COMMA || tokens[i + 2].t != token::R_BRACKET)
                return {
                    "Expected comma ',' or right bracket instead of " + tokens[i + 2].data
                };
            i += 3;
        }

        //parse funciton body
        if (tokens[i].t != token::L_F_BRACKET)
            return {
                "Expected function body instead of " + tokens[i].data
            };
        i++;
        auto r = translate_statements(tokens, i, local_vars);
        if(!r) return {r.error};

        global_funcs[func_id] = {IL_decl{func_id,return_type,true},args,r.value};

        return {""};
    }


    error::errable<void> IL_handler::translate(const std::vector<token>& tokens) {
        int i = 0;
        while (tokens[i].t != token::NONE) {
            if (tokens[i].t == token::IDENTIFIER && tokens[i + 1].t == token::IDENTIFIER) {
                // if(tokens[i].data == "namespace") {//namespaces support}
                if (tokens[i + 2].t == token::L_BRACKET) {
                    translate_function(tokens, i);
                } else {
                    auto r = parse_declaration(tokens, i, global_vars);
                    if (!r) return {r.error};
                    global_init.insert(global_init.cend(), r.value.cbegin(), r.value.cend());
                }
            }
            i++;
        }

        return {""};
    }
}

#include "IL_handler.h"

#include <stack>

namespace eraxc::IL {
    error::errable<IL_operand> translate_operand(const token& t, const scope& scope) {
        if (t.t == token::IDENTIFIER) {
            if (!scope.contains_id(t.data))
                return {"No such identifier in this scope: " + t.data, {}};
            scope::declaration decl = scope.get_declaration(t.data);
            return {"", {decl.id, decl.type, false, decl.isfunc}};
        }
        if (t.t == token::INSTANT) {
            return {"", {std::stoull(t.data), syntax::u64, false, true}};
        }
        return {"Expected identifier or instant instead of " + t.data, {}};
    }

    error::errable<std::vector<IL_node>> IL_handler::translate_expr(const std::vector<token>& tokens,
                                                                    int& i, scope& scope) {
        std::vector<IL_node> tr{};

        std::stack<IL_operand> operands{};
        std::stack<syntax::operator_type> operators{};

        auto operand0 = translate_operand(tokens[i], scope);
        if (!operand0) return {operand0.error, {}};
        operands.push(operand0.value);

        if (tokens[i + 1].t == token::OPERATOR &&
            syntax::assign_operators.contains(syntax::operators.at(tokens[i + 1].data))) {
            //assign op
            syntax::operator_type assign_type = syntax::operators.at(tokens[i + 1].data);
            auto assignee_it = scope.identifiers.find(tokens[i].data);
            i += 2;
            auto assign_to = translate_expr(tokens, i, scope);
            if (!assign_to) return assign_to;
            if (assign_type == syntax::ASSIGN) {
                assignee_it->second.id = assign_to.value.back().assignee;
            } else {
                //create new id with old type
                u64 result_id = scope.next_id++;
                assign_to.value.emplace_back(assign_to.value.back().assignee_type, result_id,
                                             IL_operand{assignee_it->second.id, assignee_it->second.type,
                                                        false, false},
                                             IL_operand{assign_to.value.back().assignee,
                                                        assign_to.value.back().assignee_type,
                                                        false, false},
                                             syntax::assign_to_common_op.at(assign_type));
                assignee_it->second.id = result_id;
            }
            return assign_to;
        }

        i++;

        while (tokens[i].t != token::SEMICOLON) {
            if (tokens[i].t == token::NONE)
                return {"Expected semicolon at the end of expression instead of EOF", {}};

            // operator then token
            if (tokens[i].t != token::OPERATOR)
                return {"Expected operator instead of " + tokens[i].data, {}};

            auto next_operand = translate_operand(tokens[i + 1], scope);
            if (!next_operand) return {next_operand.error, {}};

            //check op
            syntax::operator_type op = syntax::operators.at(tokens[i].data);
            if (syntax::assign_operators.contains(op)) return {"Cannot assign here", {}};
            if (!syntax::operator_priorities.contains(op))
                return {"Unsupported operator: " + tokens[i].data, {}};

            //while top stack operator priority > current operator priority
            while (!operators.empty() &&
                syntax::operator_priorities.at(operators.top()) < syntax::operator_priorities.at(op)) {
                //add IL_node to output from top operands and operator
                syntax::operator_type to_add = operators.top();
                operators.pop();

                //operands are on stack in backwards order so flip em
                IL_operand operand2 = operands.top();
                operands.pop();
                IL_operand operand1 = operands.top();
                operands.pop();

                // temporary: resulting type is always evaluated as top operands stack element type
                u64 result_type = operand1.type;
                u64 result_id = scope.next_id++;

                tr.emplace_back(result_type, result_id, operand1, operand2, to_add);

                //result is a new operand
                operands.push(IL_operand{result_id, result_type, false, false});
            }
            //push new operator
            operators.push(op);
            operands.push(next_operand.value);
            i += 2;
        }

        //push all left operands to result
        while (operands.size() > 1) {
            syntax::operator_type to_add = operators.top();
            operators.pop();

            //operands are on stack in backwards order so flip em
            IL_operand operand2 = operands.top();
            operands.pop();
            IL_operand operand1 = operands.top();
            operands.pop();

            // temporary: resulting type is always evaluated as left element type
            u64 result_type = operand1.type;
            u64 result_id = scope.next_id++;
            tr.emplace_back(result_type, result_id, operand1, operand2, to_add);

            operands.push(IL_operand{result_id, result_type, false, false});
        }
        if (tr.empty()) {
            tr.emplace_back(operands.top().type, scope.next_id++,
                            operands.top(), operands.top(), syntax::ASSIGN);
        }

        return {"", tr};
    }

    error::errable<std::vector<IL_node>> IL_handler::parse_declaration(const std::vector<token>& tokens,
                                                                       int& i, scope& scope) {
        if (!scope.contains_type(tokens[i].data)) return {"No such typename " + tokens[i].data, {}};
        const u64 type = scope.get_type_id(tokens[i].data);

        if (scope.cur_contains_id(tokens[i + 1].data))
            return {"Variable " + tokens[i + 1].data + " is already defined in this scope", {}};

        scope.add_id(tokens[i + 1].data, type, false);

        if (tokens[i + 2].t == token::SEMICOLON) {
            i += 2;
            return {"", {}};
        } //end of declaration
        if (tokens[i + 2].t != token::OPERATOR || tokens[i + 2].data != "=")
            return {"Expected operator= or ';' at the end of declaration", {}};
        i++; //now tokens[i] is a name of variable

        scope.next_id--;
        return translate_expr(tokens, i, scope);
    }

    //translates all statements till '}'
    error::errable<std::vector<IL_node>> IL_handler::translate_statements(const std::vector<token>& tokens,
                                                                          int& i, scope& scope) {
        std::vector<IL_node> tr;

        while (tokens[i].t != token::NONE && tokens[i].t != token::R_F_BRACKET) {
            if (tokens[i].t == token::IDENTIFIER) {
                if (tokens[i].data == "return") {
                    i++;
                    auto r = translate_expr(tokens, i, scope);
                    if (!r) return {r.error, tr};
                    tr.insert(tr.end(), r.value.begin(), --r.value.end());
                    tr.emplace_back(r.value.back().assignee_type, r.value.back().assignee,
                                    r.value.back().operand1, IL_operand{}, IL_node::RET);
                } else if (tokens[i + 1].t == token::IDENTIFIER) {
                    //decl
                    auto init = parse_declaration(tokens, i, scope);
                    if (!init) return {init.error, tr};
                    tr.insert(tr.cend(), init.value.cbegin(), init.value.cend());
                } else if (tokens[i + 1].t == token::OPERATOR) {
                    //expr
                    auto expr = translate_expr(tokens, i, scope);
                    if (!expr) return {expr.error, tr};
                    tr.insert(tr.cend(), expr.value.cbegin(), expr.value.cend());
                } else if (tokens[i + 1].t == token::L_BRACKET) {
                    //if, for, while, switch(?), function call
                }
            }
            i++;
        }
        if (tokens[i].t != token::R_F_BRACKET) return {"Expected end of function body '}' before the EOF", tr};
        return {"", tr};
    }

    error::errable<void> IL_handler::translate_function(const std::vector<token>& tokens, int& i) {
        if (!global_scope.contains_type(tokens[i].data)) return {"No such typename " + tokens[i].data};
        const u64 return_type = global_scope.get_type_id(tokens[i].data);

        if (global_scope.cur_contains_id(tokens[i + 1].data))
            return {"Variable " + tokens[i + 1].data + " is already defined in this scope"};

        const u64 func_id = global_scope.add_id(tokens[i + 1].data, return_type, true);

        scope func_scope{&global_scope};
        std::vector<IL_node> instructions{};
        std::vector<IL_operand> args{};

        i += 3;

        //parse arguments declaration
        while (tokens[i].t != token::R_BRACKET) {
            if (tokens[i].t == token::NONE) return {"Unexpected EOF in arguments list"};
            if (tokens[i].t == token::IDENTIFIER) {
                if (!func_scope.contains_type(tokens[i].data)) return {"No such typename " + tokens[i].data};
                u64 arg_type = func_scope.get_type_id(tokens[i].data);
                if (tokens[i + 1].t != token::IDENTIFIER)
                    return {"Expected variable name in arguments list instead of " + tokens[i + 1].data};
                u64 arg_id = func_scope.add_id(tokens[i + 1].data, func_scope.get_type_id(tokens[i].data), false);
                args.emplace_back(arg_id, arg_type, false);
            } else
                return {"Expected function variable list or end of function declaration instead of " + tokens[i].data};
            if (tokens[i + 2].t == token::R_BRACKET) {
                i += 2;
                break;
            }
            if (tokens[i + 2].t != token::COMMA)
                return {"Expected comma ',' or right bracket instead of " + tokens[i + 2].data};
            i += 3;
        }
        i++;
        //parse function body
        if (tokens[i].t != token::L_F_BRACKET)
            return {"Expected function body '{' instead of " + tokens[i].data};
        i++;
        auto body = translate_statements(tokens, i, func_scope);
        if (!body) return {body.error};

        global_funcs[func_id] = {IL_operand{func_id, return_type, true}, args, body.value};

        return {""};
    }


    error::errable<void> IL_handler::translate(const std::vector<token>& tokens) {
        int i = 0;
        while (i < tokens.size()) {
            if (tokens[i + 1].t == token::IDENTIFIER && tokens[i].t == token::IDENTIFIER) {
                // if(tokens[i].data == "namespace") {//namespaces support}
                if (tokens[i + 2].t == token::L_BRACKET) {
                    auto r = translate_function(tokens, i);
                    if (!r) return r;
                } else {
                    auto r = parse_declaration(tokens, i, global_scope);
                    if (!r) return {r.error};
                    global_init.insert(global_init.cend(), r.value.cbegin(), r.value.cend());
                }
                i++;
            } else
                return {"Invalid syntax encountered"};
        }
        if (!global_scope.contains_id("main") ||
            !global_scope.get_declaration("main").isfunc ||
            global_scope.get_declaration("main").type != syntax::i32)
            return {"Cannot find main function declaration: int main() {...}"};
        return {""};
    }
}
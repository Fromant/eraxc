#include "IL_handler.h"

#include <stack>

namespace eraxc::IL {


    error::errable<IL_operand> translate_var(const token &t, const scope &scope) {
        IL_operand tr{};

        if (t.t != token::IDENTIFIER && t.t != token::INSTANT)
            return {"Expected identifier or instant instead of " + t.data, {}};

        if (t.t == token::IDENTIFIER) {
            if (!scope.contains_id(t.data))
                return {"No such identifier in this scope: " + t.data, {}};
            scope::declaration decl = scope.get_declaration(t.data);
            tr.id = decl.id;
            tr.type = decl.type;
            tr.is_instant = false;
            tr.is_function = decl.isfunc;
        } else {
            //instant 100%
            tr.is_instant = true;
            tr.is_function = false;
            tr.type = syntax::u64;
            tr.id = std::stoull(t.data);
        }

        return {"", tr};
    }

    error::errable<std::pair<IL_operand, std::vector<IL_node>>> translate_expr(const std::vector<token> &tokens,
                                                                               int &i, scope &scope) {
        std::vector<IL_node> tr{};

        std::stack<IL_operand> operands{};
        std::stack<syntax::operator_type> operators{};

        auto operand0 = translate_var(tokens[i], scope);
        if (!operand0) return {operand0.error, {}};

        i++;

        operands.push(operand0.value);

        while (tokens[i].t != token::SEMICOLON) {
            if (tokens[i].t == token::NONE)
                return {"Expected semicolon at the end of expression instead of " + tokens[i].data, {}};

            // operator then token
            if (tokens[i].t != token::OPERATOR) return {"Expected operator instead of " + tokens[i].data, {}};

            auto next_operand = translate_var(tokens[i + 1], scope);
            if (!next_operand) return {next_operand.error, {}};
            operands.push(next_operand.value);

            //check op
            syntax::operator_type op = syntax::operators.at(tokens[i].data);
            if (!syntax::operator_priorities.contains(op))
                return {"Unsupported operator: " + tokens[i].data, {}};

            //while top stack operator priority >= current operator priority
            while (!operators.empty() &&
                   syntax::operator_priorities.at(operators.top()) > syntax::operator_priorities.at(op)) {
                //add IL_node to output from top operands and operator
                syntax::operator_type to_add = operators.top();
                operators.pop();

                IL_operand operand1 = operands.top();
                operands.pop();
                IL_operand operand2 = operands.top();
                operands.pop();

                // temporary: resulting type is always evaluated as top operands stack element type
                u64 result_type = operand1.type;
                u64 result_id = scope.add_id("%temp" + std::to_string(i), result_type, false);

                tr.emplace_back(result_type, result_id, operand1, operand2, to_add);

                //result is a new operand
                operands.push(IL_operand{result_id, result_type, false, false});
            }
            //push new operator
            operators.push(op);
            i += 2;
        }

        //push all left operands to result
        while (operands.size() > 1) {
            syntax::operator_type to_add = operators.top();
            operators.pop();

            IL_operand operand1 = operands.top();
            operands.pop();
            IL_operand operand2 = operands.top();
            operands.pop();

            // temporary: resulting type is always evaluated as top operands stack element type
            u64 result_type = operand1.type;
            u64 result_id = scope.add_id("%temp" + std::to_string(i), result_type, false);

            tr.emplace_back(result_type, result_id, operand1, operand2, to_add);

            //result is a new operand
            operands.push(IL_operand{result_id, result_type, false, false});
        }

        return {"", {operands.top(), tr}};
    }

    error::errable<std::vector<IL_node>> IL_handler::translate_assignment(const std::vector<token> &tokens,
                                                                          int &i, scope &scope, bool is_assigned) {
        if (tokens[i].t != token::IDENTIFIER)
            return {"Expected declaration at the start of assignment instead of " + tokens[i].data, {}};
        if (!scope.contains_id(tokens[i].data)) return {"Unknown declaration " + tokens[i].data, {}};
        scope::declaration &assignee = scope[tokens[i].data];

        syntax::operator_type assignment_type = syntax::operators.at(tokens[i + 1].data);
        i += 2;
        auto expr = translate_expr(tokens, i, scope);
        if (!expr) return {expr.error, expr.value.second};


        if (assignment_type != syntax::ASSIGN) {
            //TODO add += -= etc

            expr.value.second.emplace_back(assignee.type, assignee.id,
                                           expr.value.first, IL_operand{}, assignment_type);
        } else {
            if (is_assigned) {
                //reg new id if there's a new version of variable
                assignee.id = scope.next_id++;
                expr.value.second.emplace_back(assignee.type, assignee.id,
                                               expr.value.first, IL_operand{}, syntax::ASSIGN);
            } else if (expr.value.second.empty()) {
                expr.value.second.emplace_back(assignee.type, assignee.id,
                                               expr.value.first, IL_operand{}, syntax::ASSIGN);
            } else assignee.id = expr.value.first.id;
        }

        return {"", expr.value.second};
    }

    error::errable<std::vector<IL_node>> IL_handler::parse_declaration(const std::vector<token> &tokens,
                                                                       int &i, scope &scope) {
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
        return translate_assignment(tokens, i, scope, false);
    }

    //translates all statements till '}'
    error::errable<std::vector<IL_node>> IL_handler::translate_statements(const std::vector<token> &tokens,
                                                                          int &i, scope &scope) {
        std::vector<IL_node> tr;

        while (tokens[i].t != token::NONE && tokens[i].t != token::R_F_BRACKET) {
            if (tokens[i].t == token::IDENTIFIER) {
                if (tokens[i].data == "return") {
                    //TODO return statement
                    return {"", tr};
                } else if (tokens[i + 1].t == token::IDENTIFIER) {
                    //decl
                    auto init = parse_declaration(tokens, i, scope);
                    if (!init) return {init.error, tr};
                    tr.insert(tr.cend(), init.value.cbegin(), init.value.cend());
                } else if (tokens[i + 1].t == token::OPERATOR) {
                    //assignment
                    auto assignment = translate_assignment(tokens, i, scope, true);
                    if (!assignment) return {assignment.error, tr};
                    tr.insert(tr.cend(), assignment.value.cbegin(), assignment.value.cend());
                } else if (tokens[i + 1].t == token::L_BRACKET) {
                    //if, for, while, switch(?)
                }
            }
            i++;
        }
        if (tokens[i].t != token::R_F_BRACKET) return {"Expected end of function body '}' before the EOF", tr};
        return {"", tr};
    }

    error::errable<void> IL_handler::translate_function(const std::vector<token> &tokens, int &i) {
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
                i+=2;
                break;
            }
            if (tokens[i + 2].t != token::COMMA)
                return {"Expected comma ',' or right bracket instead of " + tokens[i + 2].data};
            i += 3;
        }

        i++;

        //parse funciton body
        if (tokens[i].t != token::L_F_BRACKET)
            return {"Expected function body '{' instead of " + tokens[i].data};
        i++;
        auto body = translate_statements(tokens, i, func_scope);
        if (!body) return {body.error};

        global_funcs[func_id] = {IL_operand{func_id, return_type, true}, args, body.value};

        return {""};
    }


    error::errable<void> IL_handler::translate(const std::vector<token> &tokens) {
        int i = 0;
        while (i < tokens.size()) {
            if (tokens[i].t == token::IDENTIFIER) {}
            if (tokens[i + 1].t == token::IDENTIFIER) {
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
            }
        }
        if (!global_scope.contains_id("main") ||
            !global_scope.get_declaration("main").isfunc ||
            global_scope.get_declaration("main").type != syntax::i32)
            return {"Cannot find main function declaration: int main() {...}"};
        return {""};
    }
}

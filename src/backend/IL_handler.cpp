#include "IL_handler.h"

#include <stack>

namespace eraxc::IL {
    error::errable<IL_operand> translate_operand(const token& t, const scope& scope) {
        if (t.t == token::IDENTIFIER) {
            if (!scope.contains_id(t.data))
                return {"No such identifier in this scope: " + t.data, {}};
            scope::declaration decl = scope.get_declaration(t.data);
            if (decl.isfunc) return {"Cannot use function there",{}};
            return {"", {decl.id, decl.type, false}};
        }
        if (t.t == token::INSTANT) {
            return {"", {std::stoull(t.data), syntax::u64, true}};
        }
        return {"Expected identifier or instant instead of " + t.data, {}};
    }

    error::errable<IL_node::Operator> translate_prefix_operation(const std::vector<token>& tokens, int& i) {
        if (tokens[i].t == token::OPERATOR && syntax::unary_operators.contains(tokens[i].data)) {
            syntax::operator_type prefix_op = syntax::unary_operators.at(tokens[i].data);
            if (prefix_op == syntax::POSITIVE) {
                //unary plus, nothing needs to be done
                //CALL means nothing here
                return {"", IL_node::CALL};
            }
            if (prefix_op == syntax::NEGATIVE) {
                return {"", IL_node::SUB};
            }
            if (prefix_op == syntax::NOT || prefix_op == syntax::BITWISE_NOT) {
                return {"", IL_node::NEG};
            }
            if (prefix_op == syntax::INCREMENT) {
                return {"", IL_node::ADD};
            }
            if (prefix_op == syntax::DECREMENT) {
                return {"", IL_node::SUB};
            }
            i++;
            return {"Unsupported operator: " + tokens[i].data, IL_node::CALL};
        }
        return {"", IL_node::CALL};
    }

    error::errable<std::vector<IL_node>> IL_handler::translate_expr(const std::vector<token>& tokens,
                                                                    int& i, scope& scope,
                                                                    const std::set<token::type>& end = {
                                                                        token::SEMICOLON}) {
        std::vector<IL_node> tr{};

        std::stack<IL_operand> operands{};
        std::stack<syntax::operator_type> operators{};

        //for postfix ops
        std::vector<std::pair<IL_node::Operator, std::pair<IL_operand, size_t>>> post_add{};

        if (tokens[i].t == token::IDENTIFIER && tokens[i + 1].t == token::OPERATOR &&
            syntax::assign_operators.contains(syntax::operators.at(tokens[i + 1].data))) {
            //assign
            syntax::operator_type assign_type = syntax::operators.at(tokens[i + 1].data);
            auto assignee_it = scope.identifiers.find(tokens[i].data);
            if (assignee_it == scope.identifiers.end())
                return {"Cannot find identifier to assign: " + tokens[i].data,
                    {}};
            i += 2;
            auto assign_to = translate_expr(tokens, i, scope);
            if (!assign_to) return assign_to;
            if (assign_type == syntax::ASSIGN) {
                assignee_it->second.id = assign_to.value.back().assignee;
            } else {
                if (assign_to.value.size() == 1 && assign_to.value.back().op == IL_node::ASSIGN) {
                    assign_to.value.back() = IL_node{assign_to.value.back().assignee_type,
                        assign_to.value.back().assignee,
                        IL_operand{assignee_it->second.id, assignee_it->second.type, false},
                        assign_to.value.back().operand1, syntax::assign_to_common_op.at(assign_type)
                    };
                    assignee_it->second.id = assign_to.value.back().assignee;
                    return assign_to;
                }
                //create new id with old type
                u64 result_id = scope.next_id++;
                assign_to.value.emplace_back(assign_to.value.back().assignee_type, result_id,
                                             IL_operand{assignee_it->second.id, assignee_it->second.type, false},
                                             IL_operand{assign_to.value.back().assignee,
                                                 assign_to.value.back().assignee_type, false},
                                             syntax::assign_to_common_op.at(assign_type));
                assignee_it->second.id = result_id;
            }
            return assign_to;
        }

        while (tokens[i].t == token::IDENTIFIER || tokens[i].t == token::L_BRACKET ||
            tokens[i].t == token::OPERATOR || tokens[i].t == token::INSTANT) {
            auto prefix = translate_prefix_operation(tokens, i);
            if (!prefix) return {prefix.error, {}};

            IL_operand operand;

            std::string s_operand;

            if (tokens[i].t == token::L_BRACKET) {
                //recursive parse
                i++;
                auto parentheses = translate_expr(tokens, i, scope, {token::R_BRACKET});
                if (!parentheses) return parentheses;
                if (parentheses.value.size() == 1 && parentheses.value.back().op == IL_node::ASSIGN) {
                    operand = parentheses.value.back().operand1;
                    scope.next_id = parentheses.value.back().assignee;
                } else {
                    tr.insert(tr.end(), parentheses.value.begin(), parentheses.value.end());
                    operand = {parentheses.value.back().assignee, parentheses.value.back().assignee_type, false};
                }
            } else if (tokens[i].t == token::INSTANT) {
                operand = IL_operand{std::stoull(tokens[i].data), syntax::u64, true};
            } else if (tokens[i].t == token::IDENTIFIER) {
                if (tokens[i + 1].t == token::L_BRACKET) {
                    //function!
                    if (!scope.contains_id(tokens[i].data))
                        return {"Unknown identifier or function: " + tokens[i].data, {}};
                    auto decl = scope.get_declaration(tokens[i].data);
                    if (!decl.isfunc) return {tokens[i].data + " is not a function", {}};
                    u64 call_result_id = scope.next_id++;
                    i += 2;
                    u64 args_passed = 0;
                    while (tokens[i].t != token::R_BRACKET) {
                        auto arg = translate_expr(tokens, i, scope, {token::R_BRACKET, token::COMMA});
                        if (!arg) return arg;

                        if (arg.value.size() == 1 && arg.value.back().op == IL_node::ASSIGN) {
                            //can be optimized
                            //return id so it can be used
                            // scope.next_id = arg.value.back().assignee;
                            //check arg type
                            if (global_funcs[decl.id].args[args_passed].type != arg.value.back().operand1.type) {
                                return {
                                    "Cannot pass arg of type " + std::to_string(arg.value.back().operand1.type) +
                                    " instead of " +
                                    std::to_string(global_funcs[decl.id].args[args_passed].type),
                                    {}};
                            }
                            tr.emplace_back(-1, -1,
                                            arg.value.back().operand1,
                                            IL_operand{args_passed, (u64)-1, false},
                                            IL_node::PASS_ARG);
                            args_passed++;
                            scope.next_id = arg.value.back().assignee;
                        } else {
                            u64 arg_type = arg.value.back().assignee_type;
                            u64 arg_id = arg.value.back().assignee;

                            //check arg type
                            if (global_funcs[decl.id].args[args_passed].type != arg_type) {
                                return {
                                    "Cannot pass arg of type " + std::to_string(arg_type) + " instead of " +
                                    std::to_string(global_funcs[decl.id].args[args_passed].type),
                                    {}};
                            }

                            args_passed++;
                            tr.insert(tr.end(), arg.value.begin(), arg.value.end());
                            tr.emplace_back(-1, -1,
                                            IL_operand{arg_id, arg_type, false},
                                            IL_operand{args_passed, (u64)-1, false},
                                            IL_node::PASS_ARG);
                        }
                    }
                    if (args_passed < global_funcs[decl.id].args.size()) {
                        return {"Not enough arguments for function: $" + std::to_string(decl.id), {}};
                    }
                    tr.emplace_back(decl.type, call_result_id,
                                    IL_operand{decl.id, decl.type, false},
                                    IL_operand{}, IL_node::CALL);

                    operand = {call_result_id, decl.type, false};
                } else {
                    s_operand = tokens[i].data;
                    auto operand_err = translate_operand(tokens[i], scope);
                    if (!operand_err) return {operand_err.error, {}};
                    operand = operand_err.value;
                }
            } else return {"Expected operand instead of: " + tokens[i].data, {}};


            if (prefix.value != IL_node::CALL) {
                u64 new_id = scope.next_id++;
                tr.emplace_back(operand.type, new_id,
                                operand,
                                IL_operand{1, syntax::u64, false},
                                prefix.value);
                if (!s_operand.empty()) scope[s_operand].id = new_id;
                operand.id = new_id;
            }
            i++;
            if (end.contains(tokens[i].t)) {
                operands.push(operand);
                break;
            }

            if (tokens[i].t != token::OPERATOR)
                return {"Expected end of expr or operator instead of " + tokens[i].data, {}};

            if (syntax::postfix_operators.contains(tokens[i].data)) {
                //postfix
                syntax::operator_type postfix_op = syntax::postfix_operators.at(tokens[i].data);
                IL_node::Operator to_add;
                if (postfix_op == syntax::INCREMENT) {
                    //increment
                    to_add = IL_node::ADD;
                } else if (postfix_op == syntax::DECREMENT) {
                    //dec
                    to_add = IL_node::SUB;
                } else return {"Unsupported postfix operator: " + tokens[i].data, {}};

                if (tokens[i - 1].t != token::IDENTIFIER)
                    return {"Cannot apply postfix operator to non identifier: " + tokens[i - 1].data, {}};
                post_add.emplace_back(to_add, std::pair{operand, i - 1});
                i++;
                if (end.contains(tokens[i].t)) {
                    operands.push(operand);
                    break;
                }
            }
            if (tokens[i].t != token::OPERATOR || !syntax::operators.contains(tokens[i].data))
                return {"Expected end of expression or operator instead of " + tokens[i].data, {}};

            syntax::operator_type op = syntax::operators.at(tokens[i].data);

            if (syntax::assign_operators.contains(op))
                return {"Cannot assign there", {}};

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
                operands.push(IL_operand{result_id, result_type, false});
            }

            operands.push(operand);
            operators.push(op);
            i++;
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

            operands.push(IL_operand{result_id, result_type, false});
        }
        if (tr.empty()) {
            tr.emplace_back(operands.top().type, scope.next_id++,
                            operands.top(), operands.top(), syntax::ASSIGN);
        }

        for (auto ta : post_add) {
            u64 new_identifier = scope.next_id++;
            tr.emplace_back(ta.second.first.type, new_identifier, ta.second.first,
                            IL_operand{1, syntax::u64, true},
                            ta.first);
            scope[tokens[ta.second.second].data].id = new_identifier;
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
            if (tokens[i].t == token::IDENTIFIER && tokens[i].data == "return") {
                //return statement;
                i++;
                auto r = translate_expr(tokens, i, scope);
                if (!r) return {r.error, tr};
                tr.insert(tr.end(), r.value.begin(), --r.value.end());
                tr.emplace_back(r.value.back().assignee_type, r.value.back().assignee,
                                r.value.back().operand1, IL_operand{}, IL_node::RET);
            } else if (tokens[i].t == token::IDENTIFIER && tokens[i].data == "if") {
                //if statement
            } else if (tokens[i].t == token::IDENTIFIER && tokens[i].data == "while") {
                //while statement
            } else if (tokens[i].t == token::IDENTIFIER && tokens[i].data == "for") {
                //for statement
            } else if (tokens[i].t == token::IDENTIFIER && tokens[i].data == "do") {
                //do {} ... statement
            } else if (tokens[i].t == token::IDENTIFIER && tokens[i + 1].t == token::IDENTIFIER) {
                //declaration
                auto init = parse_declaration(tokens, i, scope);
                if (!init) return {init.error, tr};
                tr.insert(tr.cend(), init.value.cbegin(), init.value.cend());
            } else if (tokens[i].t == token::L_BRACKET || tokens[i].t == token::OPERATOR
                || tokens[i].t == token::IDENTIFIER) {
                //expr
                auto expr = translate_expr(tokens, i, scope);
                if (!expr) return {expr.error, tr};
                tr.insert(tr.cend(), expr.value.cbegin(), expr.value.cend());
            } else return {"Unknown syntax at " + tokens[i].data, {}};
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

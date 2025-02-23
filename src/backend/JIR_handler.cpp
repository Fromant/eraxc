//
// Created by frama on 21.02.2025.
//

#include "JIR_handler.h"

#include <stack>

namespace eraxc::JIR {

    error::errable<void> JIR_handler::translate(const std::vector<token>& tokens) {
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
                    global_variables_init.insert(global_variables_init.cend(), r.value.cbegin(), r.value.cend());
                }
                i++;
            } else return {"Invalid syntax encountered"};
        }
        if (!global_scope.contains_id("main") || !global_scope.get_declaration("main").is_func ||
            global_scope.get_declaration("main").type != syntax::i32)
            return {"Cannot find main function declaration: int main() {...}"};
        return {""};
    }

    error::errable<void> JIR_handler::translate_function(const std::vector<token>& tokens, int& i) {
        if (!global_scope.contains_type(tokens[i].data)) return {"No such typename " + tokens[i].data};
        const u64 return_type = global_scope.get_type_id(tokens[i].data);

        if (global_scope.cur_contains_id(tokens[i + 1].data))
            return {"Variable " + tokens[i + 1].data + " is already defined in this scope"};

        const u64 func_id = global_scope.add_id(tokens[i + 1].data, return_type, true);

        scope func_scope {&global_scope};
        std::vector<JIR_node> instructions {};
        std::vector<JIR_operand> args {};

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
                args.emplace_back(arg_type, arg_id, false, false);
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
        if (tokens[i].t != token::L_F_BRACKET) return {"Expected function body '{' instead of " + tokens[i].data};
        i++;
        auto body = translate_statements(tokens, i, func_scope);
        if (!body) return {body.error};

        global_funcs[func_id] = JIR_function {return_type, func_id, body.value, args};

        return {""};
    }

    error::errable<JIR_op> translate_prefix_operation(const std::vector<token>& tokens, int& i) {
        if (tokens[i].t == token::OPERATOR) {
            const auto r = syntax::unary_operators.at(tokens[i].data);
            if (r == syntax::POSITIVE) return {"", JIR_op::NONE};
            if (r == syntax::NEGATIVE) return {"", JIR_op::NEG};
            if (r == syntax::NOT || r == syntax::BITWISE_NOT) return {"", JIR_op::NOT};
            if (r == syntax::INCREMENT) return {"", JIR_op::INC};
            if (r == syntax::DECREMENT) return {"", JIR_op::DEC};
            return {"Expected unary prefix operator. Got: " + tokens[i].data, {}};
        }
        return {"", JIR_op::NONE};
    }

    error::errable<JIR_operand> translate_operand(const token& t, const scope& scope) {
        if (t.t == token::IDENTIFIER) {
            if (!scope.contains_id(t.data)) return {"No such identifier in this scope: " + t.data, {}};
            scope::declaration decl = scope.get_declaration(t.data);
            if (decl.is_func) return {"Cannot use function there", {}};
            return {"", {decl.type, decl.id, false, false}};
        }
        if (t.t == token::INSTANT) { return {"", {syntax::u64, std::stoull(t.data), true, false}}; }
        return {"Expected identifier or instant instead of " + t.data, {}};
    }

    error::errable<std::vector<JIR_node>> JIR_handler::translate_expr(
        const std::vector<token>& tokens, int& i, scope& scope, const std::set<token::type>& end = {token::SEMICOLON}) {

        std::vector<JIR_node> tr {};

        std::stack<JIR_operand> operands {};
        std::stack<syntax::operator_type> operators {};

        //for postfix ops
        std::vector<std::pair<JIR_op, JIR_operand>> post_add {};

        if (tokens[i + 1].t == token::OPERATOR &&
            syntax::assign_operators.contains(syntax::operators.at(tokens[i + 1].data))) {

            u64 result_type = scope.get_declaration(tokens[i].data).type;

            auto assignee_iter = scope.identifiers.find(tokens[i].data);

            auto assignee_id = translate_operand(tokens[i], scope);
            if (!assignee_id) return {assignee_id.error, {}};
            auto assign_op = op_to_jir_op(syntax::assign_to_common_op.at(syntax::operators.at(tokens[i + 1].data)));
            if (!assign_op) return {assign_op.error, {}};
            i += 2;
            auto assign_to = translate_expr(tokens, i, scope);
            if (!assign_to) return {assign_to.error, {}};

            tr.insert(tr.end(), assign_to.value.begin(), assign_to.value.end());
            u64 result_id = assignee_id.value.value;
            if (!assign_to.value.back().operand1.is_temp && assign_op.value == JIR_op::MOVE) {
                result_id = scope.next_id++;
                tr.emplace_back(JIR_op::ALLOC, JIR_operand(result_type, result_id, false, false), JIR_operand {});
                tr.emplace_back(assign_op.value, JIR_operand(result_type, result_id, false, false),
                                assign_to.value.back().operand1);
                assignee_iter->second.id = result_id;
            } else if (assign_op.value!=JIR_op::MOVE) {
                tr.emplace_back(assign_op.value, assignee_id.value, assign_to.value.back().operand1);
            }

            return {"", tr};
        }

        while (tokens[i].t == token::IDENTIFIER || tokens[i].t == token::L_BRACKET || tokens[i].t == token::OPERATOR ||
               tokens[i].t == token::INSTANT) {
            auto prefix = translate_prefix_operation(tokens, i);
            if (!prefix) return {prefix.error, {}};

            JIR_operand operand;

            if (tokens[i].t == token::L_BRACKET) {
                //recursive parse
                i++;
                auto parentheses = translate_expr(tokens, i, scope, {token::R_BRACKET});
                if (!parentheses) return parentheses;
                tr.insert(tr.end(), parentheses.value.begin(), parentheses.value.end());
                operand = parentheses.value.back().operand1;
            } else if (tokens[i].t == token::INSTANT) {
                //TODO some float instants, select instant types (e.g. 32.f ; 64llu
                operand = JIR_operand {syntax::u64, std::stoull(tokens[i].data), true, false};
            } else if (tokens[i].t == token::IDENTIFIER) {
                if (tokens[i + 1].t == token::L_BRACKET) {
                    //function call
                    if (!scope.contains_id(tokens[i].data))
                        return {"Unknown identifier or function: " + tokens[i].data, {}};
                    auto decl = scope.get_declaration(tokens[i].data);
                    if (!decl.is_func) return {tokens[i].data + " is not a function", {}};
                    i += 2;
                    u64 args_passed = 0;
                    while (tokens[i].t != token::R_BRACKET) {
                        auto arg = translate_expr(tokens, i, scope, {token::R_BRACKET, token::COMMA});
                        if (!arg) return arg;

                        auto arg1 = arg.value.back().operand1;

                        //check arg type
                        if (global_funcs[decl.id].args[args_passed].type != arg1.type) {
                            return {"Cannot pass arg of type " + std::to_string(arg1.type) + " instead of " +
                                        std::to_string(global_funcs[decl.id].args[args_passed].type),
                                    {}};
                        }

                        args_passed++;
                        tr.insert(tr.end(), arg.value.begin(), arg.value.end());
                        tr.emplace_back(JIR_op::PASS, arg1, JIR_operand {(u64)-1, args_passed, false, false});
                    }
                    if (args_passed < global_funcs[decl.id].args.size()) {
                        return {"Not enough arguments for function: $" + std::to_string(decl.id), {}};
                    }
                    u64 call_result_id = scope.next_id++;
                    tr.emplace_back(JIR_op::ALLOC, JIR_operand {decl.type, call_result_id, false, true},
                                    JIR_operand {});
                    tr.emplace_back(JIR_op::CALL, JIR_operand {decl.type, decl.id, false, false},
                                    JIR_operand {decl.type, call_result_id, false, true});
                    operand = {decl.type, call_result_id, false, true};
                } else {
                    auto operand_err = translate_operand(tokens[i], scope);
                    if (!operand_err) return {operand_err.error, {}};
                    operand = operand_err.value;
                }
            } else return {"Expected operand instead of: " + tokens[i].data, {}};


            if (prefix.value != JIR_op::NONE) { tr.emplace_back(prefix.value, operand, JIR_operand {}); }
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
                JIR_op to_add;
                if (postfix_op == syntax::INCREMENT) {
                    //increment
                    to_add = JIR_op::INC;
                } else if (postfix_op == syntax::DECREMENT) {
                    //dec
                    to_add = JIR_op::DEC;
                } else return {"Unsupported postfix operator: " + tokens[i].data, {}};

                if (tokens[i - 1].t != token::IDENTIFIER)
                    return {"Cannot apply postfix operator to non identifier: " + tokens[i - 1].data, {}};
                post_add.emplace_back(to_add, operand);
                i++;
                if (end.contains(tokens[i].t)) {
                    operands.push(operand);
                    break;
                }
            }

            if (tokens[i].t != token::OPERATOR || !syntax::operators.contains(tokens[i].data))
                return {"Expected end of expression or operator instead of " + tokens[i].data, {}};

            syntax::operator_type op = syntax::operators.at(tokens[i].data);

            // if (syntax::assign_operators.contains(op)) return {"Cannot assign there", {}};

            while (!operators.empty() &&
                   syntax::operator_priorities.at(operators.top()) < syntax::operator_priorities.at(op)) {
                //add IL_node to output from top operands and operator
                auto to_add = op_to_jir_op(operators.top());
                if (!to_add) return {to_add.error, {}};
                operators.pop();

                //operands are on stack in backwards order so flip em
                JIR_operand operand1 = operands.top();
                operands.pop();
                JIR_operand operand2 = operands.top();
                operands.pop();

                // temporary: resulting type is always evaluated as top operands stack element type
                u64 result_type = operand1.type;
                JIR_operand result {operand1};
                if (operand1.is_const || !operand1.is_temp) {
                    u64 result_id = scope.next_id++;
                    result = {result_type, result_id, false, true};

                    tr.emplace_back(JIR_op::ALLOC, result, JIR_operand {});
                    tr.emplace_back(JIR_op::MOVE, result, operand1);
                }

                tr.emplace_back(to_add.value, result, operand2);

                //result is a new operand
                operands.push(result);
            }

            operands.push(operand);
            operators.push(op);
            i++;
        }

        //push all left operands to result
        while (operands.size() > 1) {
            auto to_add = op_to_jir_op(operators.top());
            if (!to_add) return {to_add.error, {}};
            operators.pop();

            //operands are on stack in backwards order so flip em
            JIR_operand operand2 = operands.top();
            operands.pop();
            JIR_operand operand1 = operands.top();
            operands.pop();

            // temporary: resulting type is always evaluated as top operands stack element type
            u64 result_type = operand1.type;
            JIR_operand result {operand1};
            if (operand1.is_const || !operand1.is_temp) {
                u64 result_id = scope.next_id++;
                result = {result_type, result_id, false, true};

                tr.emplace_back(JIR_op::ALLOC, result, JIR_operand {});
                tr.emplace_back(JIR_op::MOVE, result, operand1);
            }

            tr.emplace_back(to_add.value, result, operand2);

            //result is a new operand
            operands.push(result);
        }

        for (auto ta : post_add) { tr.emplace_back(std::get<0>(ta), std::get<1>(ta), JIR_operand {}); }

        if (tr.empty()) { tr.emplace_back(JIR_op::NONE, operands.top(), JIR_operand {}); }

        return {"", tr};
    }

    error::errable<std::vector<JIR_node>> JIR_handler::parse_declaration(const std::vector<token>& tokens, int& i,
                                                                         scope& scope) {
        if (!scope.contains_type(tokens[i].data)) return {"No such typename " + tokens[i].data, {}};
        const u64 type = scope.get_type_id(tokens[i].data);

        if (scope.cur_contains_id(tokens[i + 1].data))
            return {"Variable " + tokens[i + 1].data + " is already defined in this scope", {}};

        scope.add_id(tokens[i + 1].data, type, false);

        if (tokens[i + 2].t == token::SEMICOLON) {
            i += 2;
            return {"", {}};
        }  //end of declaration
        if (tokens[i + 2].t != token::OPERATOR || tokens[i + 2].data != "=")
            return {"Expected operator= or ';' at the end of declaration", {}};
        i++;  //now tokens[i] is a name of variable

        scope.next_id--;

        return translate_expr(tokens, i, scope);
    }

    //translates all statements till '}'
    error::errable<std::vector<JIR_node>> JIR_handler::translate_statements(const std::vector<token>& tokens, int& i,
                                                                            scope& scope) {
        std::vector<JIR_node> tr;

        while (tokens[i].t != token::NONE && tokens[i].t != token::R_F_BRACKET) {
            if (tokens[i].t == token::IDENTIFIER && tokens[i].data == "return") {
                //return statement;
                i++;
                auto r = translate_expr(tokens, i, scope);
                if (!r) return {r.error, tr};
                tr.insert(tr.end(), r.value.begin(), r.value.end());
                tr.emplace_back(JIR_op::RET, r.value.back().operand1, JIR_operand {});
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
            } else if (tokens[i].t == token::L_BRACKET || tokens[i].t == token::OPERATOR ||
                       tokens[i].t == token::IDENTIFIER) {
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


    error::errable<JIR_op> JIR_handler::op_to_jir_op(syntax::operator_type op) {
        if (op == syntax::ADD) return {"", JIR_op::ADD};
        if (op == syntax::SUBTRACT) return {"", JIR_op::SUB};
        if (op == syntax::MULTIPLY) return {"", JIR_op::MUL};
        if (op == syntax::DIVIDE) return {"", JIR_op::DIV};
        if (op == syntax::MODULO) return {"", JIR_op::MOD};

        if (op == syntax::INCREMENT) return {"", JIR_op::INC};
        if (op == syntax::DECREMENT) return {"", JIR_op::DEC};

        if (op == syntax::AND) return {"", JIR_op::AND};
        if (op == syntax::OR) return {"", JIR_op::OR};
        if (op == syntax::NOT) return {"", JIR_op::NOT};
        if (op == syntax::XOR) return {"", JIR_op::XOR};
        if (op == syntax::NEGATIVE) return {"", JIR_op::NEG};

        if (op == syntax::BITWISE_LSHIFT) return {"", JIR_op::LSHIFT};
        if (op == syntax::BITWISE_RSHIFT) return {"", JIR_op::RSHIFT};

        if (op == syntax::ASSIGN) return {"", JIR_op::MOVE};

        return {"Unsupported operator: " + std::to_string(op), {}};
    }


}  // eraxc::JIR
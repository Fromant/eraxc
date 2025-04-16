#include "CFG.h"

#include <iostream>

#include "errors.h"
#include "../../scope.h"
#include "../utils.h"

#include <format>


namespace eraxc::JIR {

    Scope& CFG::get_cfg_scope(const size_t node_id) {
        return scopes[nodes[node_id].scope_id];
    }

    const Scope& CFG::get_scope(size_t node_id) const {
        return scopes[nodes[node_id].scope_id];
    }


    error::errable<void> CFG::create(const std::vector<token>& tokens) {
        int i = 0;
        const size_t global_node_id = nodes.size();
        nodes.emplace_back(CFG_Node {{}, {}, scopes.size()});
        scopes.emplace_back(nullptr);

        while (i < tokens.size()) {
            if (tokens[i].t == token::IDENTIFIER && tokens[i + 1].t == token::IDENTIFIER) {
                if (tokens[i + 2].t == token::L_BRACKET) {
                    //func decl
                    auto f = parse_function(tokens, i, global_node_id);
                    if (!f) return {"Error while parsing a function:\n" + f.error};
                } else {
                    //global var decl
                    auto f = parse_declaration(tokens, i, global_node_id);
                    if (!f) return {"Error while parsing a declaration:\n" + f.error};
                }
            } else return {"Unknown statement: " + tokens[i].data};
        }
        //check for main() entrypoint
        if (!get_cfg_scope(global_node_id).contains_id("main")) return {NO_ENTRYPOINT_ERROR};
        auto main_decl = get_cfg_scope(global_node_id).get_declaration("main");
        if (!main_decl.is_func) return {NO_ENTRYPOINT_ERROR};
        if (main_decl.type != get_cfg_scope(global_node_id).get_type_id("int")) return {NO_ENTRYPOINT_ERROR};

        return {""};
    }


    error::errable<void> CFG::parse_function(const std::vector<token>& tokens, int& i, size_t node_id) {
        if (!get_cfg_scope(node_id).contains_type(tokens[i].data)) return {"No such typename " + tokens[i].data};
        const u64 return_type = get_cfg_scope(node_id).get_type_id(tokens[i].data);

        if (get_cfg_scope(node_id).cur_contains_id(tokens[i + 1].data))
            return {"Variable " + tokens[i + 1].data + " is already defined in this scope"};

        const u64 func_id = get_cfg_scope(node_id).add_id(tokens[i + 1].data, return_type, true);

        const size_t func_node_id = nodes.size();
        nodes.emplace_back(CFG_Node {{}, {}, scopes.size()});
        scopes.emplace_back(std::addressof(get_cfg_scope(node_id)));
        std::vector<Operand> args {};
        get_cfg_scope(func_node_id).father_scope = std::addressof(get_cfg_scope(node_id));

        if (tokens[i + 1].data == "main" && tokens[i].data == "return") root_id = func_node_id;

        i += 3;

        //parse arguments declaration
        while (tokens[i].t != token::R_BRACKET) {
            if (tokens[i].t == token::NONE) return {"Unexpected EOF in arguments list"};
            if (tokens[i].t == token::IDENTIFIER) {
                if (!get_cfg_scope(func_node_id).contains_type(tokens[i].data))
                    return {"No such typename " + tokens[i].data};
                u64 arg_type = get_cfg_scope(func_node_id).get_type_id(tokens[i].data);
                if (tokens[i + 1].t != token::IDENTIFIER)
                    return {"Expected variable name in arguments list instead of " + tokens[i + 1].data};
                u64 arg_id =
                    get_cfg_scope(func_node_id)
                        .add_id(tokens[i + 1].data, get_cfg_scope(func_node_id).get_type_id(tokens[i].data), false);
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
        // nodes[func_node_id].allocations.insert(nodes[func_node_id].allocations.end(), args.begin(), args.end());
        i++;
        //parse function body
        if (tokens[i].t != token::L_F_BRACKET) return {"Expected function body '{' instead of " + tokens[i].data};
        i++;

        auto body = parse_statements(tokens, i, func_node_id);
        if (!body) return {body.error};

        global_funcs[func_id] = CFG_Func {return_type, func_node_id, args};

        return {""};
    }

    error::errable<void> CFG::parse_if(const std::vector<token>& tokens, int& i, size_t node_id) {
        return {""};
    }

    error::errable<void> CFG::parse_statements(const std::vector<token>& tokens, int& i, size_t node_id) {
        //all the stuff should have CFG node arg for alloc & dealloc purposes
        while (tokens[i].t != token::R_F_BRACKET) {
            if (tokens[i].t == token::IDENTIFIER) {
                if (tokens[i].data == "return") {
                    i++;
                    auto to_return = parse_expression(tokens, i, node_id);
                    if (!to_return) return to_return.error;
                    nodes[node_id].body.emplace_back(Operation::RET, to_return.value, Operand {});
                } else if (tokens[i].data == "if") {
                    //parse if
                } else if (tokens[i].data == "for") {
                    //parse for
                } else if (tokens[i].data == "while") {
                    //parse while
                } else if (tokens[i].data == "do") {
                    //parse do
                } else {
                    if (tokens[i + 1].t == token::IDENTIFIER) {
                        //declaration
                        auto decl = parse_declaration(tokens, i, node_id);
                        if (!decl) return decl;
                        continue;
                    }
                    //expr
                    auto expr = parse_expression(tokens, i, node_id);
                    if (!expr) return expr.error;
                }
            }
        }
        i++;
        return {""};
    }

    error::errable<void> CFG::parse_declaration(const std::vector<token>& tokens, int& i, size_t node_id) {
        if (!get_cfg_scope(node_id).contains_type(tokens[i].data))
            return {"Unknown type identifier: " + tokens[i].data};
        if (get_cfg_scope(node_id).cur_contains_id(tokens[i + 1].data))
            return {"This identifier is already defined: " + tokens[i + 1].data};

        const u64 type = get_cfg_scope(node_id).get_type_id(tokens[i].data);

        get_cfg_scope(node_id).identifiers[tokens[i + 1].data] = {type, get_cfg_scope(node_id).next_id, false};

        if (tokens[i + 2].t == token::OPERATOR) {
            std::string name = tokens[i + 1].data;
            //parsing initialization
            size_t old_size = nodes[node_id].allocations.size();
            i++;
            auto init = parse_expression(tokens, i, node_id);

            if (!init) return init.error;

            if (nodes[node_id].allocations.size() == old_size) {
                const u64 id = get_cfg_scope(node_id).add_id(name, type, false);
                nodes[node_id].allocations.emplace_back(Operand(type, id, false, false));
            }

            return {""};
        }

        const u64 id = get_cfg_scope(node_id).add_id(tokens[i + 1].data, type, false);

        nodes[node_id].allocations.emplace_back(Operand(type, id, false, false));

        if (tokens[i + 2].t != token::SEMICOLON)
            return {"Expected semicolon after declaration instead of: " + tokens[i + 3].data};
        i += 3;

        return {""};
    }


    error::errable<std::pair<Operand, Nodes>> CFG::parse_expr_operand(const std::vector<token>& tokens, int& i,
                                                                      size_t node_id) {

        //Now multiple prefix & postfix operators!
        std::vector<Operation> prefix_ops;
        Nodes postfix_ops;

        while (tokens[i].t == token::OPERATOR) {
            //prefix operators
            Operation prefix_op = prefix_op_to_jir_op(tokens[i]);
            if (prefix_op == Operation::ERR)
                return {"No such prefix operator: " + tokens[i].data + ". Expected prefix operator or variable", {}};
            prefix_ops.emplace_back(prefix_op);
            i++;
        }

        if (tokens[i].t != token::IDENTIFIER)
            return {"Expected identifier in expression operand instead of: " + tokens[i].data, {}};

        auto decl = get_cfg_scope(node_id).get_declaration(tokens[i].data);
        if (decl.id == -1 && decl.type == -1) return {"Unknown identifier in this scope: " + tokens[i].data, {}};
        i++;

        Operand operand {decl.type, decl.id, false, false};
        for (auto op : prefix_ops) { nodes[node_id].body.emplace_back(op, operand, Operand {}); }

        while ((tokens[i].t == token::OPERATOR && syntax::postfix_operators.contains(tokens[i].data)) ||
               tokens[i].t == token::L_BRACKET || tokens[i].t == token::L_SQ_BRACKET) {

            if (tokens[i].t == token::L_BRACKET) {
                //function call
                if (!decl.is_func) return {"Function declaration expected", {}};
                u64 args_passed = 0;
                i++;
                //parse args
                while (tokens[i].t != token::R_BRACKET) {
                    auto arg = parse_expression(tokens, i, node_id, {token::R_BRACKET, token::COMMA});
                    if (!arg) return {"Error while parsing function call argument", {}};

                    auto arg1 = arg.value;

                    if (global_funcs[decl.id].params[args_passed].type != arg1.type) {
                        return {"Invalid argument type in function call.\nExpected: " +
                                    std::to_string(global_funcs[decl.id].params[args_passed].type) +
                                    ", got:" + std::to_string(arg1.type),
                                {}};
                    }
                    args_passed++;
                    nodes[node_id].body.emplace_back(Operation::PASS, arg1, Operand {});
                }
                if (args_passed < global_funcs[decl.id].params.size()) {
                    return {"Not enough arguments for function: $" + std::to_string(decl.id), {}};
                }

                u64 call_result_id = get_cfg_scope(node_id).next_id++;
                nodes[node_id].allocations.emplace_back(decl.type, call_result_id, false, true);
                nodes[node_id].body.emplace_back(Operation::CALL, operand,
                                                 Operand {decl.type, call_result_id, false, true});
                operand = Operand(decl.type, call_result_id, false, true);
                continue;
            }

            if (tokens[i].t == token::L_SQ_BRACKET) {
                while (tokens[i].t != token::R_SQ_BRACKET) i++;
                continue;
            }

            Operation postfix_op = postfix_op_to_jir_op(tokens[i]);
            if (postfix_op == Operation::ERR) return {"No such postfix operator: " + tokens[i].data, {}};
            i++;
            postfix_ops.emplace_back(postfix_op, operand, Operand {});
        }

        return {"", {operand, postfix_ops}};
    }

    error::errable<void> CFG::push_expr_stack(std::stack<syntax::operator_type>& operations,
                                              std::stack<Operand>& operands, size_t node_id) {
        auto to_add = op_to_jir_op(operations.top());
        if (to_add.first == Operation::ERR) return {"Invalid operation encountered."};
        operations.pop();
        //operands are on stack in backwards order so flip em
        Operand operand1 = operands.top();
        operands.pop();
        Operand operand2 = operands.top();
        operands.pop();

        // temporary: resulting type is always evaluated as top operands stack element type
        Operand result {operand1};
        result.is_rvalue = true;

        if (operand1.is_instant || !operand1.is_rvalue) {
            u64 result_id = get_cfg_scope(node_id).next_id++;
            result.value = result_id;
            result.is_instant = false;
            nodes[node_id].allocations.emplace_back(result);
            nodes[node_id].body.emplace_back(Operation::MOVE, result, operand1);
        }

        nodes[node_id].body.emplace_back(to_add.first, result, operand2);

        if (to_add.second != Operation::NONE) {
            //comparing something
            size_t jump_target_label = this->nodes.size() + 1;
            edges.emplace_back(CFG_Edge {node_id, jump_target_label, nodes[node_id].body.size()});
            nodes[node_id].body.emplace_back(to_add.second, Operand {u64(-1), jump_target_label, true, false},
                                             Operand {});
        }

        //result is a new operand
        operands.push(result);
        return {""};
    }

    error::errable<Operand> parse_instant(const token& t, Scope& scope) {
        //TODO handle out of range, etc. Best practise would be write my own parsing library with `error::errable`
        Operand tr {};
        if (t.data[t.data.size() - 2] == 'u') {
            if (t.data.back() == 'l') {
                //unsigned long, u64
                tr = {scope.get_type_id("u64"), u64(std::stoull(t.data.substr(0, t.data.size() - 2))), true, true};
            } else if (t.data.back() == 'i') {
                //unsigned integer instant, u32
                tr = {scope.get_type_id("u32"), u64(std::stol(t.data.substr(0, t.data.size() - 2))), true, true};
            } else {
                return {"Non-explicit or unknown constant type\nMake constant type explicit: e.g. `16i`", tr};
            }
        } else {
            if (t.data.back() == 'l') {
                //long, i64
                tr = {scope.get_type_id("i64"), u64(std::stol(t.data.substr(0, t.data.size() - 1))), true, true};
            } else if (t.data.back() == 'i') {
                //integer instant, i32
                tr = {scope.get_type_id("i32"), u64(std::stoi(t.data.substr(0, t.data.size() - 1))), true, true};
            } else {
                //if none is present, stick to i32
                tr = {scope.get_type_id("i32"), u64(std::stoi(t.data.substr(0, t.data.size() - 1))), true, true};
            }
            return {"Non-explicit or unknown constant type\nMake constant type explicit: e.g. `16i`", tr};
        }

        return {"", tr};
    }

    error::errable<Operand> CFG::parse_expression(const std::vector<token>& tokens, int& i, const size_t node_id,
                                                  const std::set<token::type>& end) {

        if (tokens[i + 1].t == token::OPERATOR &&
            syntax::assign_operators.contains(syntax::operators.at(tokens[i + 1].data))) {
            //Handle assign operators differently

            //parse assignee
            if (tokens[i].t != token::IDENTIFIER)
                return {"Expected variable on the left side of assign operator", Operand {}};

            auto assignee_iter = get_cfg_scope(node_id).find(tokens[i].data);
            if (assignee_iter == get_cfg_scope(node_id).identifiers.end())
                return {"Unknown identifier in assignee: " + tokens[i].data, Operand {}};

            //parse assign operation
            Operation assign_op = assign_op_to_common_op(syntax::operators.at(tokens[i + 1].data));
            if (assign_op == Operation::ERR)
                return {"??? Unsupported assign operator: " + tokens[i + 1].data + ". Please, contact devs",
                        Operand {}};

            //parse expression of what to assign to
            i += 2;
            auto assign_to = parse_expression(tokens, i, node_id, end);
            if (!assign_to) return assign_to;

            Operand tr {u64(-2), u64(-2), false, true};

            if (!assign_to.value.is_rvalue && assign_op == Operation::MOVE) {
                Operand new_assignee {assignee_iter->second.type, get_cfg_scope(node_id).next_id++, false, false};
                tr = new_assignee;
                nodes[node_id].allocations.emplace_back(new_assignee);
                nodes[node_id].body.emplace_back(assign_op, new_assignee, assign_to.value);
                assignee_iter->second.id = new_assignee.value;
            } else {
                // } else if (assign_op != Operation::MOVE) {
                tr = Operand(assignee_iter->second.type, assignee_iter->second.id, false, false);
                nodes[node_id].body.emplace_back(assign_op, tr, assign_to.value);
            }
            return {"", tr};
        }

        Nodes postfix_ops;

        std::stack<Operand> operands {};
        std::stack<syntax::operator_type> operations {};

        while (tokens[i].t == token::IDENTIFIER || tokens[i].t == token::L_BRACKET || tokens[i].t == token::OPERATOR ||
               tokens[i].t == token::INSTANT) {

            Operand operand;

            if (tokens[i].t == token::L_BRACKET) {
                //recursive parse
                i++;
                auto parentheses = parse_expression(tokens, i, node_id, {token::R_BRACKET});
                if (!parentheses) return parentheses;
                operand = parentheses.value;
            } else if (tokens[i].t == token::INSTANT) {
                auto instant = parse_instant(tokens[i], get_cfg_scope(node_id));
                if (!instant) return instant;
                operand = instant.value;
                i++;
            } else {
                auto operand_err = parse_expr_operand(tokens, i, node_id);
                if (!operand_err) return {"Error while parsing expression:\n" + operand_err.error, Operand {}};

                operand = operand_err.value.first;
                postfix_ops.insert(postfix_ops.begin(), operand_err.value.second.begin(),
                                   operand_err.value.second.end());
            }
            if (end.contains(tokens[i].t)) {
                operands.push(operand);
                break;
            }

            if (tokens[i].t != token::OPERATOR)
                return {"Expected end of expr or operator instead of " + tokens[i].data, Operand {}};
            syntax::operator_type op = syntax::operators.at(tokens[i].data);

            if (syntax::assign_operators.contains(op))
                return {"Cannot assign to rvalue $" + std::to_string(operand.value), Operand {}};

            operands.push(operand);

            while (!operations.empty() &&
                   syntax::operator_priorities.at(operations.top()) < syntax::operator_priorities.at(op)) {
                push_expr_stack(operations, operands, node_id);
            }

            operations.push(op);
            i++;
        }

        while (operands.size() > 1) {
            auto push_result = push_expr_stack(operations, operands, node_id);
            if (!push_result) return {push_result.error, Operand {}};
        }
        for (auto postfix : postfix_ops) { nodes[node_id].body.emplace_back(postfix); }

        if (end.contains(tokens[i].t)) {
            i++;
            return {"", operands.top()};
        }
        return {"Expected end of expression instead of " + tokens[i].data, Operand {}};
    }

    void CFG::print_nodes() const {
        for (const auto& node : nodes) { utils::print_JIR_nodes(node.body); }
    }

    void CFG::print_functions() const {
        for (auto func : global_funcs) {
            std::cout << func.second.return_type << " $" << func.second.node_id << " (";
            const size_t args_size = func.second.params.size();
            if (args_size > 0) {
                for (int i = 0; i < args_size - 1; i++) {
                    const auto& decl = func.second.params[i];
                    std::cout << decl.type << " $" << decl.value << ", ";
                }
                std::cout << func.second.params[args_size - 1].type << " $" << func.second.params[args_size - 1].value
                          << ") {\n";
            } else std::cout << ") {\n";

            utils::print_JIR_nodes(nodes[func.second.node_id].body);

            std::cout << "}\n";
        }
    }
}
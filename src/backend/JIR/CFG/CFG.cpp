#include "CFG.h"

#include <iostream>

#include "errors.h"
#include "../../scope.h"
#include "../utils.h"

#include <format>

namespace eraxc::JIR {

    error::errable<void> CFG::create(const std::vector<token>& tokens) {
        int i = 0;
        size_t global_node_id = nodes.size();
        nodes.emplace_back();

        while (i < tokens.size()) {
            if (tokens[i].t == token::IDENTIFIER && tokens[i + 1].t == token::IDENTIFIER) {
                if (tokens[i + 2].t == token::L_BRACKET) {
                    //func decl
                    auto f = parse_function(tokens, i, global_node_id);
                    if (!f) return {"Error while parsing a function declaration:\n" + f.error};
                } else {
                    //global var decl
                    auto f = parse_declaration(tokens, i, global_node_id);
                    if (!f) return {"Error while parsing a variable declaration:\n" + f.error};
                }
            } else return {"Unknown statement: " + tokens[i].data};
        }
        //check for main() entrypoint
        if (scopeManager.size() != 1) {
            return {"Something went wrong during compilation. Scopes count: " + std::to_string(scopeManager.size())};
        }
        if (!scopeManager.containsIdRecursive("main")) return {NO_ENTRYPOINT_ERROR};
        auto main_decl = scopeManager.findDeclaration("main");
        if (!main_decl.isFunc()) return {NO_ENTRYPOINT_ERROR};
        if (main_decl.getType() != scopeManager.findTypeRecursive("int")) return {NO_ENTRYPOINT_ERROR};

        scopeManager.dealloc_top(nodes[global_node_id].body);

        return {""};
    }


    error::errable<void> CFG::parse_function(const std::vector<token>& tokens, int& i, size_t& node_id) {
        if (!scopeManager.containsTypeRecursive(tokens[i].data)) return {"No such typename " + tokens[i].data};
        const u64 return_type = scopeManager.findTypeRecursive(tokens[i].data);

        if (scopeManager.containsId(tokens[i + 1].data))
            return {"Variable " + tokens[i + 1].data + " is already defined in this scope"};

        const u64 func_id = scopeManager.addId(tokens[i + 1].data, return_type, true, nodes[node_id].body);

        size_t func_node_id = nodes.size();
        nodes.emplace_back();

        scopeManager.push();
        std::vector<Operand> args {};

        i += 3;

        //parse arguments declaration
        while (tokens[i].t != token::R_BRACKET) {
            if (tokens[i].t == token::NONE) return {"Unexpected EOF in arguments list"};
            if (tokens[i].t == token::IDENTIFIER) {
                u64 arg_type = scopeManager.findTypeRecursive(tokens[i].data);
                if (arg_type == ScopeManager::NOT_FOUND) return {"No such typename " + tokens[i].data};

                if (tokens[i + 1].t != token::IDENTIFIER)
                    return {"Expected variable name in arguments list instead of " + tokens[i + 1].data};
                u64 arg_id = scopeManager.addIdWithoutAllocation(tokens[i + 1].data, arg_type, false);
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

        global_funcs[func_id] = CFG_Func {return_type, func_node_id, args};

        auto body = parse_statements(tokens, i, func_node_id);
        if (!body) return {body.error};

        scopeManager.dealloc_top(nodes[func_node_id].body);
        scopeManager.pop();

        return {""};
    }

    error::errable<void> CFG::parse_if(const std::vector<token>& tokens, int& i, size_t& node_id) {
        if (tokens[i + 1].t != token::L_BRACKET) return {"Expected left bracket after if: `if(cond) {body}`"};
        i += 2;

        size_t node_id_before = node_id;

        auto expr = parse_expression(tokens, i, node_id, {token::R_BRACKET});
        if (!expr) return {expr.error};

        if (node_id_before == node_id) return {"Expected conditional expression inside of if()"};

        // now node_id stands for positive branch (see push_expr_stack() for clarification)

        scopeManager.push();

        if (tokens[i].t == token::L_F_BRACKET) {
            //body
            auto body = parse_statements(tokens, i, node_id);
            if (!body) return body;
        } else {
            //single statement
            auto body = parse_statement(tokens, i, node_id);
            if (!body) return body;
        }

        scopeManager.dealloc_top(nodes[node_id].body);
        scopeManager.pop();

        Operation jump_op = jump_ops.top();
        jump_ops.pop();

        //create negative cfg branch node
        size_t negative_branch_id = nodes.size();
        nodes.emplace_back();

        edges.emplace(node_id_before, node_id);
        edges.emplace(node_id_before, negative_branch_id);

        //jump from last cfg node to negative leaving positive branch not executed
        nodes[node_id_before].body.emplace_back(jump_op, Operand {u64(-1), negative_branch_id, true, false},
                                                Operand {});

        if (tokens[i].t == token::IDENTIFIER && tokens[i].data == "else") {
            //else branch

            //create cfg node that comes after else body
            size_t new_branch_id = nodes.size();
            nodes.emplace_back();
            scopeManager.push();

            // edges.emplace(node_id, new_branch_id);
            edges.emplace(negative_branch_id, new_branch_id);

            //jump from positive to new branch leaving negative branch not executed
            nodes[node_id].body.emplace_back(Operation::JUMP, Operand {u64(-1), new_branch_id, true, false},
                                             Operand {});

            //parse else body
            i++;
            if (tokens[i].t == token::L_F_BRACKET) {
                //body
                auto body = parse_statements(tokens, i, new_branch_id);
                if (!body) return body;
            } else {
                //single statement
                auto body = parse_statement(tokens, i, new_branch_id);
                if (!body) return body;
            }

            scopeManager.dealloc_top(nodes[new_branch_id].body);
            scopeManager.pop();

            //shift current node to new branch
            node_id = new_branch_id;
        } else node_id = negative_branch_id;

        return {""};
    }

    error::errable<void> CFG::parse_statements(const std::vector<token>& tokens, int& i, size_t& node_id) {
        //all the stuff should have CFG node arg for alloc & dealloc purposes
        while (tokens[i].t != token::R_F_BRACKET) {
            auto statement = parse_statement(tokens, i, node_id);
            if (!statement) return statement;
        }
        i++;
        return {""};
    }

    error::errable<void> CFG::parse_statement(const std::vector<token>& tokens, int& i, size_t& node_id) {
        if (tokens[i].t == token::IDENTIFIER) {
            if (tokens[i].data == "return") {
                i++;
                auto to_return = parse_expression(tokens, i, node_id);
                if (!to_return) return to_return.error;

                auto& body = nodes[node_id].body;

                // scopeManager.dealloc_top(nodes[node_id].body);

                body.emplace_back(Operation::RET, to_return.value, Operand {});
            } else if (tokens[i].data == "if") {
                //parse if
                auto ifn = parse_if(tokens, i, node_id);
                if (!ifn) return ifn;
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
                    return {""};
                }
                //expr
                auto expr = parse_expression(tokens, i, node_id);
                if (!expr) return expr.error;
            }
        } else return {"Expected statement instead of: " + tokens[i].data};
        return {""};
    }

    error::errable<void> CFG::parse_declaration(const std::vector<token>& tokens, int& i, size_t node_id) {
        if (!scopeManager.containsTypeRecursive(tokens[i].data)) return {"Unknown type identifier: " + tokens[i].data};
        if (scopeManager.containsId(tokens[i + 1].data))
            return {"This identifier is already defined: " + tokens[i + 1].data};

        const u64 type = scopeManager.findTypeRecursive(tokens[i].data);

        scopeManager.addId(tokens[i + 1].data, type, false, nodes[node_id].body);

        if (tokens[i + 2].t == token::OPERATOR) {
            std::string name = tokens[i + 1].data;
            //parsing initialization
            size_t old_size = scopeManager.top().allocatedIds;
            i++;
            auto init = parse_expression(tokens, i, node_id);

            if (!init) return init.error;

            if (scopeManager.top().allocatedIds == old_size) {
                const u64 id = scopeManager.addId(name, type, false, nodes[node_id].body);
            }

            return {""};
        }

        const u64 id = scopeManager.addId(tokens[i + 1].data, type, false, nodes[node_id].body);

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

        auto decl = scopeManager.findDeclarationRecursive(tokens[i].data);
        if (decl.getId() == -1 && decl.getType() == -1)
            return {"Unknown identifier in this scope: " + tokens[i].data, {}};
        i++;

        Operand operand {decl.getType(), decl.getId(), false, false};
        for (auto op : prefix_ops) { nodes[node_id].body.emplace_back(op, operand, Operand {}); }

        while ((tokens[i].t == token::OPERATOR && syntax::postfix_operators.contains(tokens[i].data)) ||
               tokens[i].t == token::L_BRACKET || tokens[i].t == token::L_SQ_BRACKET) {

            if (tokens[i].t == token::L_BRACKET) {
                //function call
                if (!decl.isFunc()) return {"Function declaration expected", {}};
                u64 args_passed = 0;
                i++;
                //parse args
                while (tokens[i].t != token::R_BRACKET) {
                    auto arg = parse_expression(tokens, i, node_id, {token::R_BRACKET, token::COMMA});
                    if (!arg) return {"Error while parsing function call argument", {}};

                    auto arg1 = arg.value;

                    if (global_funcs[decl.getId()].params[args_passed].type != arg1.type) {
                        return {"Invalid argument type in function call.\nExpected: " +
                                    std::to_string(global_funcs[decl.getId()].params[args_passed].type) +
                                    ", got:" + std::to_string(arg1.type),
                                {}};
                    }
                    args_passed++;
                    nodes[node_id].body.emplace_back(Operation::PASS, arg1, Operand {});
                }
                if (args_passed < global_funcs[decl.getId()].params.size()) {
                    return {"Not enough arguments for function: $" + std::to_string(decl.getId()), {}};
                }

                u64 call_result_id = scopeManager.addAnonymousId(decl.getType(), false, nodes[node_id].body);
                nodes[node_id].body.emplace_back(Operation::CALL, operand,
                                                 Operand {decl.getType(), call_result_id, false, true});
                operand = Operand(decl.getType(), call_result_id, false, true);
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
                                              std::stack<Operand>& operands, size_t& node_id) {
        auto to_add = op_to_jir_op(operations.top());
        if (to_add.first == Operation::ERR) return {"Invalid operation encountered."};
        operations.pop();
        //operands are on stack in backwards order so flip em
        Operand operand1 = operands.top();
        operands.pop();
        Operand operand2 = operands.top();
        operands.pop();

        // temporary: resulting type is always evaluated as the top operands stack element type
        Operand result {operand1};
        result.is_rvalue = true;

        if (operand1.is_instant || !operand1.is_rvalue) {
            u64 result_id = scopeManager.addAnonymousId(result.type, false, nodes[node_id].body, true);
            result.value = result_id;
            result.is_instant = false;
            // nodes[node_id].allocations.emplace_back(result);
            // nodes[node_id].body.emplace_back(Operation::ALLOC, result, Operand {});
            nodes[node_id].body.emplace_back(Operation::MOVE, result, operand1);
        }

        nodes[node_id].body.emplace_back(to_add.first, result, operand2);
        if (to_add.second != Operation::NONE) {
            // conditional expression (like a>b)

            // if(...) {positive branch}
            // negative branch

            // or

            // if(...) {positive branch}
            // else {negative branch}

            //create positive cfg branch
            size_t positive_branch_id = this->nodes.size();
            nodes.emplace_back();

            jump_ops.push(to_add.second);

            //from now on we're in positive branch
            node_id = positive_branch_id;
            //after conditional positive body end, we'll go into negative cfg branch
        }

        //result is a new operand
        operands.push(result);
        return {""};
    }

    error::errable<Operand> CFG::parse_instant(const token& t) const {
        //TODO handle out of range, etc. Best practise would be write my own parsing library with `error::errable`
        Operand tr {};
        if (t.data[t.data.size() - 2] == 'u') {
            if (t.data.back() == 'l') {
                //unsigned long, u64
                tr = {scopeManager.findTypeRecursive("u64"), u64(std::stoull(t.data.substr(0, t.data.size() - 2))),
                      true, true};
            } else if (t.data.back() == 'i') {
                //unsigned integer instant, u32
                tr = {scopeManager.findTypeRecursive("u32"), u64(std::stol(t.data.substr(0, t.data.size() - 2))), true,
                      true};
            } else {
                return {"Non-explicit or unknown constant type\nMake constant type explicit: e.g. `16i`", tr};
            }
        } else {
            if (t.data.back() == 'l') {
                //long, i64
                tr = {scopeManager.findTypeRecursive("i64"), u64(std::stol(t.data.substr(0, t.data.size() - 1))), true,
                      true};
            } else if (t.data.back() == 'i') {
                //integer instant, i32
                tr = {scopeManager.findTypeRecursive("i32"), u64(std::stoi(t.data.substr(0, t.data.size() - 1))), true,
                      true};
            } else {
                //if none is present, stick to i32
                tr = {scopeManager.findTypeRecursive("i32"), u64(std::stoi(t.data.substr(0, t.data.size() - 1))), true,
                      true};
            }
        }
        return {"", tr};
    }

    error::errable<Operand> CFG::parse_expression(const std::vector<token>& tokens, int& i, size_t& node_id,
                                                  const std::set<token::type>& end) {

        if (tokens[i + 1].t == token::OPERATOR &&
            syntax::assign_operators.contains(syntax::operators.at(tokens[i + 1].data))) {
            //Handle assign operators differently

            //parse assignee
            if (tokens[i].t != token::IDENTIFIER)
                return {"Expected variable on the left side of assign operator", Operand {}};

            const auto& assignee = scopeManager.findDeclarationRecursive(tokens[i].data);
            if (assignee == ScopeManager::NOT_FOUND_DECL)
                return {"Unknown identifier in assignee: " + tokens[i].data, Operand {}};
            const std::string& assignee_name = tokens[i].data;

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

            //DO i need this assign logic??
            if (!assign_to.value.is_rvalue && assign_op == Operation::MOVE) {
                Operand new_assignee {
                    assignee.getType(),
                    // scopeManager.top().allocatedIds++,
                    scopeManager.addAnonymousId(assignee.getType(), false, nodes[node_id].body, false),
                    false, false};
                // nodes[node_id].body.emplace_back(Operation::ALLOC, new_assignee, Operand {});
                // scopeManager.addAllocation(new_assignee);
                tr = new_assignee;
                nodes[node_id].body.emplace_back(assign_op, new_assignee, assign_to.value);
                scopeManager.setDeclaration(assignee_name, {assignee.getType(), new_assignee.value, false});
            } else {
                tr = Operand(assignee.getType(), assignee.getId(), false, false);
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
                auto instant = parse_instant(tokens[i]);
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
                auto r = push_expr_stack(operations, operands, node_id);
                if (!r) return {r.error, {}};
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
        for (int i = 0; i < nodes.size(); i++) {
            std::cout << '_' << i << ":\n";
            utils::print_JIR_nodes(nodes[i].body);
        }
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
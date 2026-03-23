#include "ExpressionParser.hpp"

#include <stack>

using namespace eraxc::frontend;
using namespace eraxc;
using namespace error;

static errable<JIR::Operand> parse_instant(const std::string& instant) {
    if (instant.empty()) {
        return {"Error while parsing instant: empty string", {}};
    }

    std::string suffix;
    Keyword type = Keyword::i32;
    bool is_unsigned = false;

    // Check 2-character suffixes first to avoid partial matches (e.g. "ul" vs "l")
    if (instant.size() >= 2) {
        std::string end2 = instant.substr(instant.size() - 2);
        if (end2 == "ul") {
            type = Keyword::u64;
            suffix = "ul";
            is_unsigned = true;
        } else if (end2 == "ui") {
            type = Keyword::u32;
            suffix = "ui";
            is_unsigned = true;
        } else if (end2 == "us") {
            type = Keyword::u16;
            suffix = "us";
            is_unsigned = true;
        } else if (end2 == "uc") {
            type = Keyword::u8;
            suffix = "uc";
            is_unsigned = true;
        }
    }

    // If no 2-char suffix matched, check 1-character suffixes
    if (suffix.empty()) {
        char last = instant.back();
        if (last == 'l') {
            type = Keyword::i64;
            suffix = "l";
            is_unsigned = false;
        } else if (last == 'u') {
            type = Keyword::u32;
            suffix = "u";
            is_unsigned = true;
        } else if (last == 'i') {
            type = Keyword::i32;
            suffix = "i";
            is_unsigned = false;
        } else if (last == 's') {
            type = Keyword::i16;
            suffix = "s";
            is_unsigned = false;
        } else if (last == 'b') {
            type = Keyword::u8;
            suffix = "b";
            is_unsigned = true;
        } else if (last == 'c') {
            type = Keyword::i8;
            suffix = "c";
            is_unsigned = false;
        }
        // else default i32 (suffix remains empty)
    }

    // Extract the numeric part
    std::string number_part = instant.substr(0, instant.size() - suffix.size());

    // Check for floating point indicators in the numeric part
    if (number_part.find('.') != std::string::npos || number_part.find('e') != std::string::npos ||
        number_part.find('E') != std::string::npos) {
        return {"Float instants are unsupported", {}};
    }

    try {

        const auto jirType = jirTypeFromKeyword(type);

        if (jirType == JIR::Type::ERR) {
            return {"Cannot convert type " + std::to_string((u64)type) + " to JIR type.", {}};
        }

        if (is_unsigned) {
            return {"", JIR::Operand {jirType, std::stoull(number_part), true, true}};
        }
        return {"", JIR::Operand {jirType, (u64)std::stoll(number_part), true, true}};
    } catch (const std::exception& exception) {
        return {std::string("Error while parsing instant: ") + exception.what(), {}};
    }
}

errable<ExpressionParser::ParseResult> ExpressionParser::parse(const std::vector<Token>& tokens, size_t& pos,
                                                               const std::set<Token::type>& endTokens) {
    std::vector<JIR::Command> tr;

    if (tokens[pos + 1].t == Token::OPERATOR && assign_operators.contains(operators.at(tokens[pos + 1].data))) {
        //Handle assign operators differently

        //parse assignee
        if (tokens[pos].t != Token::IDENTIFIER) {
            return {"Expected variable on the left side of assign operator", {}};
        }

        const auto& assigneeOpt = scope_manager.findDeclarationRecursive(tokens[pos].data);
        if (!assigneeOpt) {
            return {"Unknown identifier in assignee: " + tokens[pos].data, {}};
        }
        const auto& assignee = assigneeOpt.value();
        const std::string& assignee_name = tokens[pos].data;

        //parse assign operation
        JIR::Operation assign_op = assignOpToCommonOp(operators.at(tokens[pos + 1].data));
        if (assign_op == JIR::Operation::ERR) {
            return {"??? Unsupported assign operator: " + tokens[pos + 1].data + ". Please, contact devs", {}};
        }

        //parse expression of what to assign to
        pos += 2;
        auto assign_to = parse(tokens, pos, endTokens);
        if (!assign_to) {
            return assign_to;
        }

        //DO I need this assign logic??
        if (!assign_to.value.result.is_rvalue && assign_op == JIR::Operation::MOVE) {
            tr.emplace_back(assign_op, assign_to.value.result, assign_to.value.result);
            scope_manager.setDeclaration(assignee_name, {assignee.getType(), assign_to.value.result.value, false});
            return {"", assign_to.value};
        }

        const auto jirType = assignee.getJirType();

        if (!jirType) {
            return {jirType.error, {}};
        }

        auto expr_res = JIR::Operand(jirType.value, assignee.getId(), false, false);
        tr.emplace_back(assign_op, expr_res, assign_to.value.result);
        return {"", {tr, expr_res}};
    }

    std::vector<JIR::Command> postfix_ops;

    std::stack<JIR::Operand> operands {};
    std::stack<OperatorType> operations {};

    while (tokens[pos].t == Token::IDENTIFIER || tokens[pos].t == Token::L_BRACKET ||
           tokens[pos].t == Token::OPERATOR || tokens[pos].t == Token::INSTANT) {

        if (tokens[pos].t == Token::OPERATOR && prefix_operators.contains(tokens[pos].data)) {
            auto operand_err = parse_expr_operand(tokens, pos, tr, postfix_ops);
            if (!operand_err) {
                return {"Error while parsing expression:\n" + operand_err.error, {}};
            }
            operands.push(operand_err.value);
            if (endTokens.contains(tokens[pos].t)) {
                break;
            }
            if (tokens[pos].t != Token::OPERATOR) {
                return {"Expected end of expr or operator instead of " + tokens[pos].data, {}};
            }
            OperatorType op = operators.at(tokens[pos].data);
            if (assign_operators.contains(op)) {
                return {"Cannot assign to rvalue $" + std::to_string(operand_err.value.value), {}};
            }
            while (!operations.empty() && operator_priorities.at(operations.top()) < operator_priorities.at(op)) {
                auto r = push_expr_stack(operations, operands, tr);
                if (!r) {
                    return {r.error, {}};
                }
            }
            operations.push(op);
            pos++;
            continue;
        }

        JIR::Operand operand {};

        if (tokens[pos].t == Token::L_BRACKET) {
            //recursive parse
            pos++;
            auto parentheses = parse(tokens, pos, {Token::R_BRACKET});
            if (!parentheses) {
                return parentheses;
            }
            operand = parentheses.value.result;
            tr.insert(tr.end(), parentheses.value.node.begin(), parentheses.value.node.end());
        } else if (tokens[pos].t == Token::INSTANT) {
            auto instant = parse_instant(tokens[pos].data);
            if (!instant) {
                return {instant.error, {}};
            }
            operand = instant.value;
            pos++;
        } else {
            auto operand_err = parse_expr_operand(tokens, pos, tr, postfix_ops);
            if (!operand_err) {
                return {"Error while parsing expression:\n" + operand_err.error, {}};
            }

            operand = operand_err.value;
        }
        if (endTokens.contains(tokens[pos].t)) {
            operands.push(operand);
            break;
        }

        if (tokens[pos].t != Token::OPERATOR) {
            return {"Expected end of expr or operator instead of " + tokens[pos].data, {}};
        }
        OperatorType op = operators.at(tokens[pos].data);

        if (assign_operators.contains(op)) {
            return {"Cannot assign to rvalue $" + std::to_string(operand.value), {}};
        }

        operands.push(operand);

        while (!operations.empty() && operator_priorities.at(operations.top()) < operator_priorities.at(op)) {
            auto r = push_expr_stack(operations, operands, tr);
            if (!r) {
                return {r.error, {}};
            }
        }

        operations.push(op);
        pos++;
    }

    while (operands.size() > 1) {
        auto push_result = push_expr_stack(operations, operands, tr);
        if (!push_result) {
            return {push_result.error, {}};
        }
    }
    for (auto& postfix : postfix_ops) {
        tr.emplace_back(postfix);
    }

    if (endTokens.contains(tokens[pos].t)) {
        pos++;
        if (operands.empty()) {
            return {"Empty expression not expected", {}};
        }
        return {"", {tr, operands.top()}};
    }
    return {"Expected end of expression instead of " + tokens[pos].data, {}};
};

errable<void> ExpressionParser::push_expr_stack(std::stack<OperatorType>& operations,
                                                std::stack<JIR::Operand>& operands,
                                                std::vector<JIR::Command>& cmds) const {
    auto to_add = opToJirOp(operations.top());
    if (to_add == JIR::Operation::ERR) {
        return {"Invalid operation encountered."};
    }
    operations.pop();
    //operands are on stack in backwards order so flip em
    JIR::Operand operand2 = operands.top();
    operands.pop();
    JIR::Operand operand1 = operands.top();
    operands.pop();

    // temporary: resulting type is always evaluated as the top operands stack element type
    JIR::Operand expr_res {operand1};
    expr_res.is_rvalue = true;

    if (operand1.is_instant || operand1.is_rvalue) {
        cmds.emplace_back(to_add, operand1, operand2);
        expr_res.value = operand1.value;
        expr_res.is_instant = operand1.is_instant;
        expr_res.is_rvalue = true;
    } else {
        const auto type = keywordFromJirType(expr_res.type);
        u64 result_id = scope_manager.addAnonymousId((u64)type, false, true);
        expr_res.value = result_id;
        expr_res.is_instant = false;
        expr_res.is_rvalue = true;
        //copy operand1
        cmds.emplace_back(JIR::Operation::MOVE, expr_res, operand1);
        cmds.emplace_back(to_add, expr_res, operand2);
    }

    //result is a new operand
    operands.push(expr_res);
    return {""};
}

errable<JIR::Operand> ExpressionParser::parse_expr_operand(const std::vector<Token>& tokens, size_t& pos,
                                                           std::vector<JIR::Command>& cmds,
                                                           std::vector<JIR::Command>& postfix_cmds) {

    std::vector<JIR::Operation> prefix_ops;

    while (tokens[pos].t == Token::OPERATOR) {
        JIR::Operation prefix_op = prefixOpToJirOp(tokens[pos]);
        if (prefix_op == JIR::Operation::ERR) {
            break;
        }
        prefix_ops.emplace_back(prefix_op);
        pos++;
    }

    JIR::Operand operand {};

    if (tokens[pos].t == Token::L_BRACKET) {
        pos++;
        auto parentheses = parse(tokens, pos, {Token::R_BRACKET});
        if (!parentheses) {
            return {"Error parsing parenthesized expression: " + parentheses.error, {}};
        }
        operand = parentheses.value.result;
        cmds.insert(cmds.end(), parentheses.value.node.begin(), parentheses.value.node.end());
    } else if (tokens[pos].t == Token::INSTANT) {
        auto instant = parse_instant(tokens[pos].data);
        if (!instant) {
            return {instant.error, {}};
        }
        operand = instant.value;
        pos++;
    } else if (tokens[pos].t == Token::IDENTIFIER) {
        auto declOpt = scope_manager.findDeclarationRecursive(tokens[pos].data);
        if (!declOpt) {
            return {"Unknown identifier in this scope: " + tokens[pos].data, {}};
        }
        const auto& decl = declOpt.value();

        const auto declJirType = decl.getJirType();
        if (!declJirType) {
            return {declJirType.error, {}};
        }
        operand = JIR::Operand {declJirType.value, decl.getId(), false, false};
        pos++;

        while ((tokens[pos].t == Token::OPERATOR && postfix_operators.contains(tokens[pos].data)) ||
               tokens[pos].t == Token::L_BRACKET || tokens[pos].t == Token::L_SQ_BRACKET) {

            if (tokens[pos].t == Token::L_BRACKET) {
                if (!decl.isFunc()) {
                    return {"Function expected for call operator", {}};
                }
                u64 args_passed = 0;
                pos++;

                const auto& function_iter = scope_manager.getFunctions().find(decl.getId());
                if (function_iter == scope_manager.getFunctions().end()) {
                    return {"Internal error - function is not registered", {}};
                }

                const auto& function = function_iter->second;

                if (tokens[pos].t == Token::R_BRACKET) {
                    pos++;
                } else {
                    while (tokens[pos].t != Token::R_BRACKET) {
                        auto arg = parse(tokens, pos, {Token::COMMA, Token::R_BRACKET});
                        if (!arg) {
                            return {"Error while parsing function call argument" + arg.error, {}};
                        }

                        auto arg1 = arg.value;

                        if (args_passed < function.params.size()) {
                            const auto requiredType = function.params[args_passed].type;
                            if (requiredType != arg1.result.type) {
                                return {"Invalid argument type in function call.\nExpected: " + std::to_string((u64)requiredType) +
                                            ", got:" + std::to_string((u64)arg1.result.type),
                                        {}};
                            }
                        }
                        args_passed++;
                        cmds.insert(cmds.end(), arg1.node.begin(), arg1.node.end());
                        cmds.emplace_back(JIR::Operation::PASS, arg1.result, JIR::Operand {});

                        if (tokens[pos].t == Token::COMMA) {
                            pos++;
                        }
                    }
                    pos++;
                }

                if (args_passed < function.params.size()) {
                    return {"Not enough arguments for function: $" + std::to_string(decl.getId()), {}};
                }

                u64 call_result_id = scope_manager.addAnonymousId(decl.getType(), false, true);
                const auto declT = decl.getJirType();
                if (!declT) {
                    return {declT.error, {}};
                }
                cmds.emplace_back(JIR::Operation::CALL, operand, JIR::Operand {declT.value, call_result_id, false, true});
                operand = JIR::Operand(declT.value, call_result_id, false, true);
                continue;
            }

            if (tokens[pos].t == Token::L_SQ_BRACKET) {
                while (tokens[pos].t != Token::R_SQ_BRACKET) {
                    pos++;
                }
                pos++;
                continue;
            }

            JIR::Operation postfix_op = postfixOpToJirOp(tokens[pos]);
            if (postfix_op == JIR::Operation::ERR) {
                return {"No such postfix operator: " + tokens[pos].data, {}};
            }
            pos++;
            postfix_cmds.emplace_back(postfix_op, operand, JIR::Operand {});
        }
    } else {
        if (!prefix_ops.empty()) {
            return {"Expected identifier or expression after prefix operator instead of: " + tokens[pos].data, {}};
        }
        return {"Expected identifier in expression operand instead of: " + tokens[pos].data, {}};
    }

    for (auto op : prefix_ops) {
        cmds.emplace_back(op, operand, JIR::Operand {});
    }

    return {"", operand};
}

errable<ExpressionParser::ParseCondResult> ExpressionParser::parse_cond(const std::vector<Token>& tokens, size_t& pos,
                                                                        const std::set<Token::type>& endTokens) {
    std::vector<JIR::Command> tr;

    if (tokens[pos + 1].t == Token::OPERATOR && assign_operators.contains(operators.at(tokens[pos + 1].data))) {
        return {"cannot assign inside condition", {}};
    }

    std::vector<JIR::Command> postfix_ops;

    std::stack<JIR::Operand> operands {};
    std::stack<OperatorType> operations {};

    while (tokens[pos].t == Token::IDENTIFIER || tokens[pos].t == Token::L_BRACKET ||
           tokens[pos].t == Token::OPERATOR || tokens[pos].t == Token::INSTANT) {

        if (tokens[pos].t == Token::OPERATOR && prefix_operators.contains(tokens[pos].data)) {
            auto operand_err = parse_expr_operand(tokens, pos, tr, postfix_ops);
            if (!operand_err) {
                return {"Error while parsing expression:\n" + operand_err.error, {}};
            }
            operands.push(operand_err.value);
            if (endTokens.contains(tokens[pos].t)) {
                break;
            }
            if (tokens[pos].t != Token::OPERATOR) {
                return {"Expected end of expr or operator instead of " + tokens[pos].data, {}};
            }
            OperatorType op = operators.at(tokens[pos].data);
            if (assign_operators.contains(op)) {
                return {"Cannot assign to rvalue $" + std::to_string(operand_err.value.value), {}};
            }
            while (!operations.empty() && operator_priorities.at(operations.top()) < operator_priorities.at(op)) {
                auto r = push_cond_expr_stack(operations, operands, tr);
                if (!r) {
                    return {r.error, {}};
                }
            }
            operations.push(op);
            pos++;
            continue;
        }

        JIR::Operand operand {};

        if (tokens[pos].t == Token::L_BRACKET) {
            //recursive parse
            pos++;
            auto parentheses = parse(tokens, pos, {Token::R_BRACKET});
            if (!parentheses) {
                return {parentheses.error, {}};
            }
            operand = parentheses.value.result;
            tr.insert(tr.end(), parentheses.value.node.begin(), parentheses.value.node.end());
        } else if (tokens[pos].t == Token::INSTANT) {
            auto instant = parse_instant(tokens[pos].data);
            if (!instant) {
                return {instant.error, {}};
            }
            operand = instant.value;
            pos++;
        } else {
            auto operand_err = parse_expr_operand(tokens, pos, tr, postfix_ops);
            if (!operand_err) {
                return {"Error while parsing expression:\n" + operand_err.error, {}};
            }

            operand = operand_err.value;
        }
        if (endTokens.contains(tokens[pos].t)) {
            operands.push(operand);
            break;
        }

        if (tokens[pos].t != Token::OPERATOR) {
            return {"Expected end of expr or operator instead of " + tokens[pos].data, {}};
        }
        OperatorType op = operators.at(tokens[pos].data);

        if (assign_operators.contains(op)) {
            return {"Cannot assign to rvalue $" + std::to_string(operand.value), {}};
        }

        operands.push(operand);

        while (!operations.empty() && operator_priorities.at(operations.top()) < operator_priorities.at(op)) {
            auto r = push_cond_expr_stack(operations, operands, tr);
            if (!r) {
                return {r.error, {}};
            }
        }

        operations.push(op);
        pos++;
    }

    // TODO test for a < b < c (should fail)

    while (operands.size() > 1) {
        auto push_result = push_cond_expr_stack(operations, operands, tr);
        if (!push_result) {
            return {push_result.error, {}};
        }
    }
    for (auto& postfix : postfix_ops) {
        tr.emplace_back(postfix);
    }

    if (endTokens.contains(tokens[pos].t)) {
        pos++;
        return {"",
                {
                    tr,
                }};
    }
    return {"Expected end of expression instead of " + tokens[pos].data, {}};
}

errable<JIR::Operation> ExpressionParser::push_cond_expr_stack(std::stack<OperatorType>& operations,
                                                               std::stack<JIR::Operand>& operands,
                                                               std::vector<JIR::Command>& cmds) const {
    auto to_add = cmpOpToJirOp(operations.top());
    if (to_add == JIR::Operation::ERR) {
        return {"Invalid compare operation encountered.", to_add};
    }
    operations.pop();
    //operands are on stack in backwards order so flip em
    JIR::Operand operand2 = operands.top();
    operands.pop();
    JIR::Operand operand1 = operands.top();
    operands.pop();

    // temporary: resulting type is always evaluated as the top operands stack element type
    JIR::Operand result {operand1};
    result.is_rvalue = true;

    // TODO rvalue logic and logic at all

    if (!operand1.is_instant && operand1.is_rvalue) {
        cmds.emplace_back(to_add, operand1, operand2);
        result.value = operand1.value;
        result.is_instant = operand1.is_instant;
    } else {
        const auto& keyword = keywordFromJirType(result.type);
        u64 result_id = scope_manager.addAnonymousId((u64)keyword, false, true);
        result.value = result_id;
        result.is_instant = false;
        //copy operand1
        cmds.emplace_back(JIR::Operation::MOVE, result, operand1);
        cmds.emplace_back(to_add, result, operand2);
        // const auto& keyword2 = keywordFromJirType(operand1.type);
        // scope_manager.setDeclaration(operand1.value, Scope::Declaration {(u64)keyword2, result.value, false});
    }

    //result is a new operand
    operands.push(result);
    return {"", to_add};
}
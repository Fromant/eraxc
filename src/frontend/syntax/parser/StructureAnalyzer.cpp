#include "StructureAnalyzer.hpp"

#include "common/CFG/CFGIterator.hpp"

using namespace eraxc::frontend;

/// Function to parse
/// @param tokens vector of tokens
/// @param pos position of where function declaration starts. After execution is set to first token after function
/// @return error if found
error::errable<eraxc::JIR::Function> StructureAnalyzer::parseFunction(const std::vector<Token>& tokens, size_t& pos) {
    const auto return_type = scopeManager.findTypeRecursive(tokens[pos].data);
    if (!return_type) {
        return {"No such typename " + tokens[pos].data, {}};
    }

    const auto funcId = scopeManager.addIdWithoutAllocation(tokens[pos + 1].data, (size_t)return_type.value(), true);
    if (!funcId) {
        return {"Identifier " + tokens[pos + 1].data + " is already defined in this scope", {}};
    }

    if (tokens[pos + 2].t != Token::L_BRACKET) {
        return {"Expected `([params])` after function declaration", {}};
    }

    pos += 3;

    scopeManager.pushFrame();

    std::vector<JIR::Declaration> params {};

    while (tokens[pos].t != Token::R_BRACKET) {
        if (tokens[pos].t == Token::NONE) {
            return {"Unexpected EOF in arguments list", {}};
        }
        if (tokens[pos].t == Token::IDENTIFIER) {
            auto arg_type = scopeManager.findTypeRecursive(tokens[pos].data);
            if (!arg_type) {
                return {"No such typename " + tokens[pos].data, {}};
            }
            if (tokens[pos + 1].t != Token::IDENTIFIER) {
                return {"Expected variable name in arguments list instead of " + tokens[pos + 1].data, {}};
            }
            auto arg_id = scopeManager.addIdWithoutAllocation(tokens[pos + 1].data, arg_type.value(), false);
            if (!arg_id) {
                return {"Cannot allocate parameter somehow", {}};
            }
            params.emplace_back(arg_id.value(), jirTypeFromKeyword((Keyword)arg_type.value()));
        } else {
            return {"Expected function parameters declaration or end of function declaration instead of " +
                        tokens[pos].data,
                    {}};
        }
        if (tokens[pos + 2].t == Token::R_BRACKET) {
            pos += 2;  // break if parsed last argument
            break;
        }
        if (tokens[pos + 2].t != Token::COMMA) {
            return {"Expected comma ',' or right bracket instead of " + tokens[pos + 2].data, {}};
        }
        pos += 3;  //skip parsed parameter and comma
    }

    pos++;  // skip right bracket

    JIR::Function to_add {};
    to_add.decl = JIR::Declaration(funcId.value(), jirTypeFromKeyword((Keyword)return_type.value()));
    to_add.params = params;

    // register function so it could be called inside itself
    scopeManager.getFunctions().emplace(to_add.decl.id, to_add);

    // parse function body
    if (tokens[pos].t != Token::L_F_BRACKET) {
        return {"Expected function body '{' instead of " + tokens[pos].data, {}};
    }

    size_t node_id = 0;
    to_add.cfg.nodes.emplace_back();
    auto body = parseStatements(tokens, pos, to_add.cfg, node_id);
    if (!body) {
        return {body.error, {}};
    }
    //TODO check & add return if needed
    const auto stackSize = scopeManager.popFrame();
    if (!stackSize) {
        return {stackSize.error, {}};
    }
    to_add.cfg.maxStackSize = stackSize.value;
    result.functions.emplace_back(to_add);

    return {"", to_add};
}

error::errable<void> StructureAnalyzer::parseStatements(const std::vector<Token>& tokens, size_t& pos, CFG::CFG& cfg,
                                                        size_t& node_id) {
    if (tokens[pos].t != Token::L_F_BRACKET) {
        return parseStatement(tokens, pos, cfg, node_id);
    }

    pos++;  // skip '{'

    while (tokens[pos].t != Token::R_F_BRACKET) {
        auto statement = parseStatement(tokens, pos, cfg, node_id);
        if (!statement) {
            return statement;
        }
    }
    pos++;  // skip '}'
    return "";
}

error::errable<void> StructureAnalyzer::parseStatement(const std::vector<Token>& tokens, size_t& pos, CFG::CFG& cfg,
                                                       size_t& node_id) {
    if (tokens[pos].t != Token::IDENTIFIER) {
        return {"Expected statement instead of: " + tokens[pos].data};
    }

    if (tokens[pos].data == "return") {
        pos++;
        auto expr_err = parser.parse(tokens, pos);
        if (!expr_err) {
            return expr_err.error;
        }

        // just dealloc because scope and frame will be popped later on '}' symbol
        // should call all the destructors or something
        scopeManager.deallocFrame(expr_err.value.node);

        const auto& expr = expr_err.value.node;
        cfg.nodes[node_id] += expr;
        cfg.nodes[node_id].commands.emplace_back(JIR::Operation::PASS_RET, expr_err.value.result, JIR::Operand {});
        cfg.nodes[node_id].commands.emplace_back(JIR::Operation::RET, JIR::Operand {}, JIR::Operand {});
        return "";
    }
    if (tokens[pos].data == "if") {
        // parse if
        return parseIf(tokens, pos, cfg, node_id);
    }
    if (tokens[pos].data == "while") {
        // parse while
        return parseWhile(tokens, pos, cfg, node_id);
    }
    if (tokens[pos].data == "for") {
        // parse for
        return {"For is unsupported now"};
    }
    if (tokens[pos].data == "do") {
        // parse do
        return {"Do is unsupported now"};
    }

    if (tokens[pos + 1].t == Token::IDENTIFIER) {
        // declaration
        auto decl = parseDeclaration(tokens, pos, cfg, node_id);
        if (!decl) {
            return decl.error;
        }
        return "";
    }
    // expression
    const auto expr_err = parser.parse(tokens, pos);
    if (!expr_err) {
        return expr_err.error;
    }
    const auto& expr = expr_err.value.node;
    cfg.nodes[node_id] += expr;
    return "";
}

error::errable<void> StructureAnalyzer::parseIf(const std::vector<Token>& tokens, size_t& pos, CFG::CFG& cfg,
                                                size_t& node_id) {
    if (tokens[pos].t != Token::IDENTIFIER || tokens[pos].data != "if") {
        return "Expected `if` at the start of if statement";
    }
    if (tokens[pos + 1].t != Token::L_BRACKET) {
        return "Expected left bracket after if: `if(condition expr) {body}`";
    }
    pos += 2;

    // parse condition

    auto cond_err = parser.parse(tokens, pos, {Token::R_BRACKET});
    if (!cond_err) {
        //TODO no condition appeared error
        return cond_err.error;
    }

    auto& cond = cond_err.value.node;

    // append condition to current node
    cfg.nodes[node_id] += cond;

    const size_t node_id_before = node_id;

    // create positive branch
    size_t positive_branch = cfg.nodes.size();
    cfg.nodes.emplace_back();

    size_t negative_branch = cfg.nodes.size();
    cfg.nodes.emplace_back();

    // scope for positive branch
    scopeManager.push();

    // jump to branches
    cfg.nodes[node_id].commands.emplace_back(JIR::Operation::CMP, cond_err.value.result,
                                             JIR::Operand {JIR::Type::I32, 1, true, true});
    cfg.edges[node_id_before].emplace_back(positive_branch, CFG::CFGEdge::EXTEND, JIR::Operation::JE);
    cfg.edges[node_id_before].emplace_back(negative_branch, CFG::CFGEdge::EXTEND);

    // parse positive branch body
    auto positive_body = parseStatements(tokens, pos, cfg, positive_branch);
    if (!positive_body) {
        return positive_body;
    }

    // end of scope of positive branch
    scopeManager.pop();

    if (tokens[pos].t == Token::IDENTIFIER && tokens[pos].data == "else") {
        // else branch
        // parse else body
        pos++;

        // scope for negative branch
        scopeManager.push();

        const size_t else_body_id = negative_branch;
        node_id = else_body_id;
        auto else_body = parseStatements(tokens, pos, cfg, negative_branch);
        if (!else_body) {
            return else_body;
        }

        // end of else scope
        scopeManager.pop();

        const size_t after_body_id = cfg.nodes.size();
        cfg.nodes.emplace_back();
        cfg.edges[negative_branch].emplace_back(after_body_id, CFG::CFGEdge::SQUASH);
        cfg.edges[positive_branch].emplace_back(after_body_id, CFG::CFGEdge::SQUASH);
    } else {
        // if body to after
        const size_t after_body_id = negative_branch;
        cfg.edges[positive_branch].emplace_back(after_body_id, CFG::CFGEdge::SQUASH);
        node_id = after_body_id;
    }

    return "";
}
error::errable<void> StructureAnalyzer::parseWhile(const std::vector<Token>& tokens, size_t& pos, CFG::CFG& cfg,
                                                   size_t& node_id) {
    if (tokens[pos].t != Token::IDENTIFIER || tokens[pos].data != "while") {
        return "Expected `while` at the start of while statement";
    }
    if (tokens[pos + 1].t != Token::L_BRACKET) {
        return "Expected left bracket after `while`: `while(condition expr) {body}`";
    }
    pos += 2;

    auto cond = parser.parse(tokens, pos, {Token::R_BRACKET});
    if (!cond) {
        return cond.error;
    }

    const auto& cond_body = cond.value.node;

    if (cond_body.commands.empty()) {
        //TODO no condition appeared error
        return "Expected condition expression inside of while()";
    }

    // create cond branch
    const size_t cond_branch = cfg.nodes.size();
    cfg.nodes.emplace_back(cond_body);  //copy cond body
    cfg.edges[node_id].emplace_back(cond_branch, CFG::CFGEdge::EXTEND);

    // create all branches
    const size_t while_body = cfg.nodes.size();
    cfg.nodes.emplace_back();
    const size_t after_body = cfg.nodes.size();
    cfg.nodes.emplace_back();

    cfg.nodes[cond_branch].commands.emplace_back(JIR::Operation::CMP, cond.value.result,
                                                 JIR::Operand {JIR::Type::I32, 1, true, true});
    cfg.edges[cond_branch].emplace_back(while_body, CFG::CFGEdge::EXTEND, JIR::Operation::JE);
    cfg.edges[cond_branch].emplace_back(after_body, CFG::CFGEdge::SQUASH);

    scopeManager.push();

    node_id = while_body;
    auto body = parseStatements(tokens, pos, cfg, node_id);

    if (!body) {
        return body;
    }

    cfg.edges[node_id].emplace_back(cond_branch, CFG::CFGEdge::SQUASH);
    scopeManager.pop();

    node_id = after_body;

    return "";
}


error::errable<void> StructureAnalyzer::parseDeclaration(const std::vector<Token>& tokens, size_t& pos, CFG::CFG& cfg,
                                                         const size_t& node_id) {
    const auto typeOpt = scopeManager.findTypeRecursive(tokens[pos].data);
    if (!typeOpt) {
        return {"Unknown type identifier: " + tokens[pos].data};
    }

    const u64 type = typeOpt.value();
    const std::string name = tokens[pos + 1].data;

    if (scopeManager.containsId(tokens[pos + 1].data)) {
        return {"This identifier is already defined: " + tokens[pos + 1].data};
    }

    if (tokens[pos + 2].t == Token::OPERATOR) {
        if (tokens[pos + 2].data != "=") {
            return "Only `=` operator allowed inside declaration initialization. Example: `i32 a = 2;`";
        }

        //parsing initialization (e.g. `int a = a + 2`)
        pos += 3;
        // parse a + 2
        auto init_val = parser.parse(tokens, pos);

        if (!init_val) {
            return init_val.error;
        }
        const auto& expr = init_val.value.node;
        const auto& expr_res = init_val.value.result;

        if (expr_res.is_rvalue && !expr_res.is_instant) {
            scopeManager.linkId(name, expr_res.value, type, false);
        } else {
            const auto id = scopeManager.addId(name, type, false, false, cfg.nodes[node_id]);
            if (!id) {
                return {"Something went wrong during declaration initialization parsing..."};
            }
            const JIR::Operand operand {jirTypeFromKeyword((Keyword)type), id.value(), false, false};

            cfg.nodes[node_id].commands.emplace_back(JIR::Operation::MOVE, operand, expr_res);
        }

        cfg.nodes[node_id] += expr;

        return "";
    }
    scopeManager.addId(name, type, false, false, cfg.nodes[node_id]);

    if (tokens[pos + 2].t != Token::SEMICOLON) {
        return {"Expected semicolon after declaration instead of: " + tokens[pos + 2].data};
    }
    pos += 3;

    return "";
}

error::errable<eraxc::JIR::FrontendResult> StructureAnalyzer::analyze(const std::vector<Token>& tokens) {

    // analyze token stream
    // file at top-level may consist only of global declarations and functions for now
    // typenames, classes, structs, templates, concepts, imports and exports will be coming soon

    size_t i = 0;

    while (i < tokens.size()) {
        if (tokens[i].t == Token::IDENTIFIER && tokens[i + 1].t == Token::IDENTIFIER) {
            //global decl or function decl
            if (tokens[i + 2].t == Token::L_BRACKET) {
                //function declaration
                auto r = parseFunction(tokens, i);
                if (!r) {
                    return {r.error, {}};
                }
            } else {
                result.globals.emplace_back();
                //global variable declaration
                size_t node_id = 0;
                result.globals.back().init_cfg.nodes.emplace_back();
                auto r = parseDeclaration(tokens, i, result.globals.back().init_cfg, node_id);
                if (!r) {
                    return {r.error, {}};
                }
            }
        } else {
            return {"Unknown statement: " + tokens[i].data, {}};
        }
    }

    const auto main_id = scopeManager.findIdRecursive("main");
    if (!main_id) {
        return {"Cannot find main()", {}};
    }

    result.entrypointId = main_id.value();
    // for (size_t fn = 0; fn < result.functions.size(); fn++) {
    //     const auto& f = result.functions[fn];
    //     if (f.decl.id == main_id.value()) {
    //         // found main
    //         break;
    //     }
    // }

    return {"", result};
}

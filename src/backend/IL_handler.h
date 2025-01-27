#ifndef ERAXC_IL_HANDLER_H
#define ERAXC_IL_HANDLER_H

#include <vector>
#include <set>

#include "../frontend/lexic/preprocessor_tokenizer.h"
#include "../frontend/syntax/enums.h"
#include "../util/error.h"
#include "scope.h"

typedef unsigned long long u64;

namespace eraxc::IL {
    struct IL_node {
        enum Operator : int {
            ASSIGN, //=MOV
            ADD, SUB, MUL, DIV,
            NOT, NEG,
            OR, AND, XOR,
            JUMP, CALL, RET,
            //TODO compare nodes
        };

        u64 op1 = -1;
        u64 op2 = -1;
        Operator op = ASSIGN;
        bool isOp1Instant = false;
        bool isOp2Instant = false;

        IL_node(u64 op1, u64 op2, syntax::operator_type op, bool isOp1Instant, bool isOp2Instant) {
            this->op1 = op1;
            this->op2 = op2;
            this->isOp1Instant = isOp1Instant;
            this->isOp2Instant = isOp2Instant;

            if (op == syntax::ADD) this->op = ADD;
            else if (op == syntax::SUBTRACT) this->op = SUB;
            else if (op == syntax::DIVIDE) this->op = DIV;
            else if (op == syntax::MULTIPLY) this->op = MUL;
        }
    };

    struct IL_decl {
        u64 id;
        u64 type;
        bool is_function;
    };


    struct IL_func {
        IL_decl declaration;
        std::vector<IL_decl> args;
        std::vector<IL_node> body;
    };


    /// Handler of Intermediate Language. Computes all the types and may optimise a bit
    struct IL_handler {
        scope global_scope{nullptr};
        std::unordered_map<u64, IL_func> global_funcs{};

        std::vector<IL_node> global_init{};

        error::errable<std::vector<IL_node>> translate_expr(const std::vector<token> &tokens,
                                                            int &i, const scope &scope);

        error::errable<std::vector<IL_node>> translate_statements(const std::vector<token> &tokens,
                                                                  int &i, scope &scope);

        error::errable<std::vector<IL_node>> parse_declaration(const std::vector<token> &tokens,
                                                               int &i, scope &scope);

        error::errable<void> translate(const std::vector<token> &tokens);

        error::errable<void> translate_function(const std::vector<token> &tokens, int &i);
    };
}

#endif

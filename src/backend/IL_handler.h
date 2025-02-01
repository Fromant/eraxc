#ifndef ERAXC_IL_HANDLER_H
#define ERAXC_IL_HANDLER_H

#include <vector>
#include <set>

#include "../frontend/lexic/preprocessor_tokenizer.h"
#include "../frontend/syntax/enums.h"
#include "../util/error.h"
#include "scope.h"

namespace eraxc::IL {

    struct IL_operand {
        u64 id = -1;
        u64 type = -1;
        bool is_function = false;
        bool is_instant = false;
    };


    struct IL_node {
        enum Operator : int {
            ASSIGN, //=MOV
            ADD, SUB, MUL, DIV,
            NOT, NEG,
            OR, AND, XOR,
            JUMP, CALL, RET,
        };
        u64 assignee_type = -1;
        u64 assignee = -1;

        IL_operand operand1;
        IL_operand operand2;

        Operator op = ASSIGN;

        IL_node(u64 assignee_type, u64 assignee,
                IL_operand operand1, IL_operand operand2,
                syntax::operator_type op) {
            this->assignee_type = assignee_type;
            this->assignee = assignee;

            this->operand1 = operand1;
            this->operand2 = operand2;

            if (op == syntax::ADD) this->op = ADD;
            else if (op == syntax::SUBTRACT) this->op = SUB;
            else if (op == syntax::DIVIDE) this->op = DIV;
            else if (op == syntax::MULTIPLY) this->op = MUL;
        }
    };

    struct IL_func {
        IL_operand declaration;
        std::vector<IL_operand> args;
        std::vector<IL_node> body;
    };


    /// Handler of Intermediate Language. Computes all the types and may optimise a bit
    struct IL_handler {
        scope global_scope{nullptr};
        std::unordered_map<u64, IL_func> global_funcs{};

        std::vector<IL_node> global_init{};

        static error::errable<std::vector<IL_node>> translate_assignment(const std::vector<token> &tokens,
                                                                         int &i, scope &scope, bool is_assigned);

        static error::errable<std::vector<IL_node>> translate_statements(const std::vector<token> &tokens,
                                                                         int &i, scope &scope);

        static error::errable<std::vector<IL_node>> parse_declaration(const std::vector<token> &tokens,
                                                                      int &i, scope &scope);

        error::errable<void> translate(const std::vector<token> &tokens);

        error::errable<void> translate_function(const std::vector<token> &tokens, int &i);
    };
}

#endif

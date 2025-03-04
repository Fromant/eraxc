//
// Created by frama on 21.02.2025.
//

#ifndef JIR_HANDLER_H
#define JIR_HANDLER_H

#include "scope.h"
#include "../frontend/lexic/preprocessor_tokenizer.h"

typedef unsigned long long u64;

namespace eraxc::JIR {

    enum class JIR_op {
        ADD,
        SUB,
        MUL,
        DIV,
        MOD,

        INC,
        DEC,

        NOT,
        NEG,
        AND,
        OR,
        XOR,
        LSHIFT,
        RSHIFT,

        CMP,

        MOVE,
        CALL,
        RET,
        LABEL,
        JUMP,
        JE,
        JNE,
        JG,
        JGE,
        JL,
        JLE,
        PASS,
        ALLOC,
        STORE,
        LOAD,  //(?)

        NONE
    };

    struct JIR_operand {
        u64 type;
        u64 value;
        bool is_const;
        bool is_temp;

        JIR_operand(u64 type, u64 val, bool is_const, bool is_temp) {
            this->type = type;
            this->value = val;
            this->is_const = is_const;
            this->is_temp = is_temp;
        }

        JIR_operand() {
            type = -1;
            value = -1;
            is_const = false;
            is_temp = false;
        }
    };

    struct JIR_node {
        JIR_op operation;
        JIR_operand operand1;
        JIR_operand operand2;
    };

    struct JIR_function {
        u64 return_type;
        u64 id;
        std::vector<JIR_node> body;
        std::vector<JIR_operand> args;
    };

    struct JIR_handler {
        //replace with Control flow graph one day
        //all functions list
        std::unordered_map<u64, JIR_function> global_funcs;

        //Global scope
        scope global_scope {nullptr};

        //THREAD UNSAFE VAR
        JIR_operand jump_target_label;

        //Global vars init
        std::vector<JIR_node> global_variables_init;

        error::errable<void> translate(const std::vector<token>& tokens);

        error::errable<void> translate_function(const std::vector<token>& tokens, int& i);

        error::errable<std::vector<JIR_node>> translate_statements(const std::vector<token>& tokens, int& i,
                                                                   scope& func_scope);

        error::errable<std::vector<JIR_node>> translate_expr(const std::vector<token>& tokens, int& i, scope& scope,
                                                             const std::set<token::type>& end);

        error::errable<std::vector<JIR_node>> parse_declaration(const std::vector<token>& tokens, int& i, scope& scope);

        static error::errable<std::pair<JIR_op, JIR_op>> op_to_jir_op(syntax::operator_type op);
    };


}  // eraxc::JIR

#endif  //JIR_HANDLER_H

#ifndef ERAXC_IL_HANDLER_H
#define ERAXC_IL_HANDLER_H

#include <vector>
#include <set>

#include "../frontend/syntax/AST.h"

typedef unsigned long long u64;

namespace eraxc::IL {

    struct IL_node {
        enum type {
            ADD, MOV, SUB, MUL, DIV, IMUL, NOT, OR, AND, XOR,
            NONE
        };
        u64 op1 = -1;
        u64 op2 = -1;
        type type = NONE;
        bool isOp2Instant = false;
        bool isOp1Instant = false;

    };

    std::vector<IL_node> translate(const syntax::AST::statement_node &statement);

    std::vector<IL_node> translate(const syntax::AST::expr_node &expr);

    struct IL_decl {
        enum type : unsigned char {
            b8, b16, b32, b64, //length in bits
            NONE
        };
        u64 id;
        type type;

        IL_decl(u64 id, u64 type) {
            this->id = id;
            this->type = get_type(type);
        }

        explicit IL_decl(const syntax::AST::decl_node &decl) {
            this->id = decl.id;
            this->type = get_type(decl.type);
        }

        static enum type get_type(u64 type);

        bool operator<(const IL_decl &other) const {
            return id < other.id;
        }
    };


    struct IL_func {
        IL_decl declaration;
        std::vector<IL_decl> args;
        std::vector<IL_node> body;

        bool operator<(const IL_func &other) const {
            return declaration.id < other.declaration.id;
        }

        IL_func(u64 id, u64 type, const std::vector<syntax::AST::decl_node> &args,
                const std::vector<syntax::AST::statement_node> &body) : declaration(id, type) {

            for (const auto &arg: args) {
                this->args.emplace_back(arg);
            }

            for (const auto &stat: body) {
                auto ti = translate(stat);
                this->body.insert(this->body.cend(), ti.cbegin(), ti.cend());
            }
        }
    };


    /// Handler of Intermediate Language. Computes all the types and may optimise a bit
    struct IL_handler {

        std::set<IL_decl> global_vars{};
        std::set<IL_func> funcs{};
        std::vector<IL_node> global_init{};

        explicit IL_handler(const std::vector<syntax::AST::node> &nodes) {
            for (auto &a: nodes) {
                if (a.type == syntax::AST::node::DECLARATION) {
                    global_vars.emplace(a.d.decl.id, a.d.decl.type);
                    if (a.d.decl.assign_to.op != syntax::NONE) {
                        const auto &ti = translate(a.d.decl.assign_to);
                        global_init.insert(global_init.cend(), ti.cbegin(), ti.cend());
                    }
                } else if (a.type == syntax::AST::node::FUNCTION) {
                    funcs.emplace(a.d.func->id, a.d.func->return_typename, a.d.func->args, a.d.func->body);
                } else {
                    std::cerr << "Bad syntax tree provided, no such node supported: " << a.type << std::endl;
                    exit(-12);
                }
            }
        }
    };


}

#endif

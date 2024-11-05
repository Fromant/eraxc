#ifndef BLCK_COMPILER_AST_H
#define BLCK_COMPILER_AST_H

#include "syntax_analyzer.h"
#include "enums.h"

namespace blck::syntax::AST {

    struct expr_node {
        operator_type op = NONE;

        bool isInstant = false;
        size_t data = 0xFFFFFFFF;

        //data
        expr_node *right = nullptr;

        expr_node() = default;

        expr_node(operator_type op, size_t data, bool isInstant) {
            this->op = op;
            this->data = data;
            this->isInstant = isInstant;
        };

        void print() const {
            auto it = this;

            while (it->right != nullptr) {
                if (it->isInstant) {
                    std::cout << it->data << ' ' << find_op(it->op) << ' ';
                } else {
                    std::cout << '{' << it->data << "} " << find_op(it->op) << ' ';
                }
                it = it->right;
            }
            if (it->op != NONE) {
                if (it->isInstant) {
                    std::cout << it->data << ' ';
                } else {
                    std::cout << '{' << it->data << "} ";
                }
            }
            std::cout << ';' << '\n';
        }
    };

    struct decl_node {
        size_t id = -1;
        size_t type = -1;
        expr_node assign_to{};

        void print() const {
            std::cout << get_typename(type) << ":{" << id <<'}';
            if (assign_to.op != operator_type::NONE) {
                std::cout << " = ";
                assign_to.print();
            } else std::cout << ';' << '\n';
        }
    };

    struct statement_node {
        enum statement_type {
            DECLARATION, EXPRESSION, RETURN, SELECTION, LOOP, NONE
        };

        statement_type type = NONE;

        union statement_data {
            expr_node *expr = nullptr;
            decl_node decl;
//                return_node ret;
//                selection_onde sel;
//                loop_node loop;
        };

        void print() const {
            if (type == DECLARATION) {
                data.decl.print();
            } else if (type == EXPRESSION) {
                data.expr->print();
            }
        }

        statement_data data;
    };

    struct func_node {
        size_t id;
        size_t return_typename;
        std::vector<decl_node> args;
        std::vector<statement_node> body;

        void print() {
            std::cout << get_typename(return_typename) << ' ' << get_identifier(id) << "(";
            //print parameters
            const int n = (int) args.size();
            if (n > 0) {
                for (int i = 0; i < n - 1; i++) {
                    std::cout << get_typename(args[i].type) << " {" << args[i].id << "}, ";
                }
                std::cout << get_typename(args[n - 1].type) << " {" << args[n - 1].id << '}';
            }
            std::cout << ')' << ' ' << '{' << '\n';
            for (statement_node &stat: body) {
                stat.print();
            }
            std::cout << "}\n";
        }
    };

    struct node {
        enum node_type {
            DECLARATION, STATEMENT, EXPRESSION, FUNCTION, NONE
        };
        node_type type = NONE;
        union data {
            decl_node decl;
            expr_node expr;
            func_node *func = nullptr;
        };
        data d;

        void print() const;
    };
}

#endif //BLCK_COMPILER_AST_H

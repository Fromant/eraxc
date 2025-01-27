#ifndef ERAXC_SYNTAX_ANALYZER_H
#define ERAXC_SYNTAX_ANALYZER_H


#include <set>
#include <string>
#include <utility>
#include <iostream>

#include "lexic.h"
#include "../frontend/syntax/enums.h"
#include "../frontend/syntax/AST.h"


namespace eraxc::syntax {

    static inline std::unordered_map<std::string, size_t> string_instants{};

    struct scope {

        scope *father_scope = nullptr;

        size_t id_offset = 0;

        explicit scope(scope *const father) {
            if (father == nullptr) {
                //global scope
                typenames = std::unordered_map<std::string, size_t>{
                        {"i8",    i8},
                        {"i16",   i16},
                        {"i32",   i32},
                        {"i64",   i64},
                        {"i128",  i128},
                        {"i256",  i256},

                        {"u8",    u8},
                        {"u16",   u16},
                        {"u32",   u32},
                        {"u64",   u64},
                        {"u128",  u128},
                        {"u256",  u256},

                        {"int",   INT},
                        {"long",  LONG},
                        {"char",  CHAR},
                        {"bool",  BOOL},
                        {"short", SHORT},
                        {"void",  VOID}
                };
            } else id_offset = father->identifiers.size() + father->id_offset;
            father_scope = father;
        }

        bool contains_type(const std::string &type) {
            auto it = this;
            while (it != nullptr) {
                if (it->typenames.contains(type)) return true;
                it = it->father_scope;
            }
            return false;
        }

        bool cur_contains_id(const std::string &id) const {
            return identifiers.contains(id);
        }

        bool contains_id(const std::string &id) {
            auto it = this;
            while (it != nullptr) {
                if (it->identifiers.contains(id)) return true;
                it = it->father_scope;
            }
            return false;
        }

        size_t get_type(const std::string &type) {
            auto it = this;
            while (it != nullptr) {
                if (it->typenames.contains(type)) return it->typenames[type];
                it = it->father_scope;
            }
            return -1;
        }

        size_t get_id(const std::string &id) {
            auto it = this;
            while (it != nullptr) {
                if (it->identifiers.contains(id)) return it->identifiers[id];
                it = it->father_scope;
            }
            return -1;
        }

        /// Function to add identifier into scope
        /// \param id identifier to add
        /// \return the index of identifier
        size_t add_id(const std::string &id) {
            size_t tr = identifiers.size() + id_offset;
            identifiers[id] = tr;
            return tr;
        }

        std::unordered_map<std::string, size_t> identifiers{};
        std::unordered_map<std::string, size_t> typenames{};
    };


    struct syntax_analyzer {
        scope global_scope{nullptr};


        const std::string &get_typename(size_t id) const;
        const std::string &get_identifier(size_t id) const;

        error::errable<std::vector<AST::node>> analyze(const std::vector<lexic::Token> &ts);

        static error::errable<AST::expr_node *>
        parse_expr_node(const std::vector<lexic::Token> &ts, size_t &i, scope &scope);

        static error::errable<AST::expr_node *>
        parse_expr(const std::vector<lexic::Token> &ts, size_t &i, scope &scope, lexic::Token::type end);

        static error::errable<std::vector<AST::statement_node>>
        parse_func_body(const std::vector<lexic::Token> &ts, size_t &i, scope &scope);

        error::errable<AST::func_node *>
        parse_func(const std::vector<lexic::Token> &ts, size_t &i, scope &scope) const;

        static error::errable<void>
        parse_function_definition(const std::vector<lexic::Token> &ts, size_t &i, scope &global_scope);

        static error::errable<AST::decl_node> parse_decl(const std::vector<lexic::Token> &ts,
                                                         size_t &i,
                                                         scope &scope,
                                                         lexic::Token::type);
    };
}

#endif //ERAXC_SYNTAX_ANALYZER_H

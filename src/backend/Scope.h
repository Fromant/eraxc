#ifndef BLCK_COMPILER_SCOPE_H
#define BLCK_COMPILER_SCOPE_H

#include <unordered_map>
#include <string>

#include "../frontend/syntax/enums.h"

namespace eraxc {

    typedef unsigned long long u64;

    struct Scope {

        struct declaration {
            u64 type = -1;
            u64 id = -1;
            bool is_func = false;
        };

        Scope* father_scope = nullptr;

        u64 next_id = 0;

        explicit Scope(Scope* const father) {
            if (father == nullptr) {
                //global scope
                typenames = std::unordered_map<std::string, size_t> {
                    {"i8", syntax::i8},     {"i16", syntax::i16},   {"i32", syntax::i32},
                    {"i64", syntax::i64},   {"i128", syntax::i128}, {"i256", syntax::i256},

                    {"u8", syntax::u8},     {"u16", syntax::u16},   {"u32", syntax::u32},
                    {"u64", syntax::u64},   {"u128", syntax::u128}, {"u256", syntax::u256},

                    {"int", syntax::i32},   {"long", syntax::i64},  {"char", syntax::i8},
                    {"bool", syntax::BOOL}, {"short", syntax::i16}, {"void", syntax::VOID}};
            } else next_id = father->next_id;
            father_scope = father;
        }

        Scope() = delete;

        bool contains_type(const std::string& type) const {
            auto it = this;
            while (it != nullptr) {
                if (it->typenames.contains(type)) return true;
                it = it->father_scope;
            }
            return false;
        }

        bool cur_contains_id(const std::string& id) const { return identifiers.contains(id); }

        bool contains_id(const std::string& id) const {
            auto it = this;
            while (it != nullptr) {
                if (it->identifiers.contains(id)) return true;
                it = it->father_scope;
            }
            return false;
        }

        size_t get_type_id(const std::string& type) const {
            auto it = this;
            while (it != nullptr) {
                if (it->typenames.contains(type)) return it->typenames.at(type);
                it = it->father_scope;
            }
            return -1;
        }

        /// Function for getting declaration id in scope
        /// \param id string of declaration
        /// \return declaration id
        declaration get_declaration(const std::string& id) const {
            auto it = this;
            while (it != nullptr) {
                if (it->identifiers.contains(id)) return it->identifiers.at(id);
                it = it->father_scope;
            }
            return {size_t(-1), size_t(-1)};
        }

        /// Function to add declaration into scope
        /// \param id declaration to add
        /// \param type id of type of added identifier
        /// \param is_func is this identifier a function
        /// \return the index of declaration
        size_t add_id(const std::string& id, size_t type, bool is_func) {
            size_t tr = next_id;
            identifiers[id] = {type, next_id, is_func};
            next_id++;
            return tr;
        }

        std::unordered_map<std::string, declaration>::iterator find(const std::string& name) {
            Scope* it = this;
            while (it != nullptr) {
                auto tr = it->identifiers.find(name);
                if (tr != it->identifiers.end()) return tr;
                it = it->father_scope;
            }
            return identifiers.end();
        }

        declaration& operator[](const std::string& id) { return identifiers[id]; }

        std::unordered_map<std::string, declaration> identifiers {};
        std::unordered_map<std::string, size_t> typenames {};
    };
}
#endif  //BLCK_COMPILER_SCOPE_H

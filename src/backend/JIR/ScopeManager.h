#pragma once

#include "Node.h"
#include "Operand.h"
#include "Operation.h"
#include "CFG/CFG_parts.h"

#include "backend/Scope.h"
#include "frontend/syntax/enums.h"

#include <ranges>

namespace eraxc::JIR {
    class ScopeManager {
        std::vector<Scope> scopes;

        std::vector<std::vector<Operand>> allocations;

        typedef std::vector<Node> Nodes;

    public:
        ScopeManager() {
            scopes.emplace_back();
            allocations.emplace_back();
            //init global scope with default types
            scopes.back().typenames = std::unordered_map<std::string, size_t> {
                {"i8", syntax::i8},     {"i16", syntax::i16},   {"i32", syntax::i32},
                {"i64", syntax::i64},   {"i128", syntax::i128}, {"i256", syntax::i256},

                {"u8", syntax::u8},     {"u16", syntax::u16},   {"u32", syntax::u32},
                {"u64", syntax::u64},   {"u128", syntax::u128}, {"u256", syntax::u256},

                {"int", syntax::i32},   {"long", syntax::i64},  {"char", syntax::i8},
                {"bool", syntax::BOOL}, {"short", syntax::i16}, {"void", syntax::VOID}};
        }

        size_t size() { return scopes.size(); }

        u64 addType(const std::string& type) {
            u64 tr = top().typenames.size();
            top().typenames.emplace(type, tr);
            return tr;
        }

        void addTypes(const std::vector<std::string>& typenames) {
            for (const auto& type : typenames) { addType(type); }
        }

        bool containsTypeRecursive(const std::string& type) const {
            for (const auto& scope : std::ranges::views::reverse(scopes)) {
                if (scope.typenames.contains(type)) return true;
            }
            return false;
        }

        bool containsType(const std::string& type) const { return top().typenames.contains(type); }

        bool containsIdRecursive(const std::string& id) const {
            for (const auto& scope : std::ranges::views::reverse(scopes)) {
                if (scope.identifiers.contains(id)) return true;
            }
            return false;
        }

        bool containsId(const std::string& id) const { return top().identifiers.contains(id); }

        static constexpr u64 NOT_FOUND = -1llu;

        u64 findIdRecursive(const std::string& id) const {
            for (const auto& scope : std::ranges::views::reverse(scopes)) {
                if (const auto it = scope.identifiers.find(id); it != scope.identifiers.end()) {
                    return it->second.getId();
                }
            }
            return NOT_FOUND;
        }

        u64 findId(const std::string& id) const {
            if (const auto it = top().identifiers.find(id); it != top().identifiers.end()) {
                return it->second.getId();
            }
            return NOT_FOUND;
        }

        static inline const Scope::Declaration NOT_FOUND_DECL {NOT_FOUND, NOT_FOUND, false};

        auto findDeclarationRecursive(const std::string& name) const {
            for (auto& scope : std::ranges::views::reverse(scopes)) {
                if (auto it = scope.identifiers.find(name); it != scope.identifiers.end()) { return it->second; }
            }
            return NOT_FOUND_DECL;
        }

        auto findDeclaration(const std::string& name) const {
            if (auto it = top().identifiers.find(name); it != top().identifiers.end()) { return it->second; }
            return NOT_FOUND_DECL;
        }

        void setDeclaration(const std::string& name, const Scope::Declaration& decl) { top().identifiers[name] = decl; }

        u64 findTypeRecursive(const std::string& type) const {
            for (const auto& scope : std::ranges::views::reverse(scopes)) {
                if (auto it = scope.typenames.find(type); it != scope.typenames.end()) { return it->second; }
            }
            return NOT_FOUND;
        }

        u64 findType(const std::string& type) const {
            if (auto it = top().typenames.find(type); it != top().typenames.end()) return it->second;
            return NOT_FOUND;
        }

        /// Function to add declaration into scope
        /// \param id declaration to add
        /// \param type id of type of added identifier
        /// \param is_func is this identifier a function
        /// \param nodes nodes list where allocation operation will be added
        /// \return the index of declaration
        size_t addId(const std::string& id, size_t type, bool is_func, Nodes& nodes, bool rValue = false) {
            top().identifiers.emplace(id, Scope::Declaration {type, top().allocatedIds, is_func});
            if (!is_func) {
                Operand allocatee {type, top().allocatedIds, false, rValue};
                allocations.back().emplace_back(allocatee);
                nodes.emplace_back(Operation::ALLOC, allocatee, Operand {});
            }
            return top().allocatedIds++;
        }

        size_t addIdWithoutAllocation(const std::string& id, size_t type, bool is_func, bool rValue = false) {
            top().identifiers.emplace(id, Scope::Declaration {type, top().allocatedIds, is_func});
            return top().allocatedIds++;
        }

        size_t addAnonymousId(const u64 type, bool is_func, Nodes& nodes, bool rValue = false) {
            u64& id = top().allocatedIds;
            top().identifiers.emplace("$anonymous" + std::to_string(id), Scope::Declaration {type, id, is_func});
            if (!is_func) {
                Operand allocatee {type, id, false, rValue};
                allocations.back().emplace_back(allocatee);
                nodes.emplace_back(Operation::ALLOC, allocatee, Operand {});
            }
            return id++;
        }

        void addAllocation(const Operand& op) { allocations.back().emplace_back(op); }

        Scope& top() { return scopes.back(); }
        const Scope& top() const { return scopes.back(); }

        const auto& top_allocations() const { return allocations.back(); }

        void push() {
            scopes.emplace_back();
            allocations.emplace_back();
            if (scopes.size() > 1) { scopes.back().allocatedIds = scopes[scopes.size() - 2].allocatedIds; }
        }

        void dealloc_top(Nodes& nodes) {

            auto& allocs = allocations.back();
            for (auto& allocatee : std::views::reverse(allocs)) {
                nodes.emplace_back(Operation::DEALLOC, allocatee, Operand {});
            }

            // auto& scope = scopes.back();

            // for (auto& id : scope.identifiers) {
            // if (id.second.isFunc()) continue;
            // Operand dealloc {id.second.getType(), id.second.getId(), false, false};
            // nodes.emplace_back(Operation::DEALLOC, dealloc, Operand {});
            // }
        }

        void pop() {
            scopes.pop_back();
            allocations.pop_back();
        }
    };

}
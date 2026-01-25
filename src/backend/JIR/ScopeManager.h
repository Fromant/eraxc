#pragma once

#include <algorithm>
#include <ranges>
#include <stack>

#include "JIRop.h"
#include "Operand.h"
#include "Operation.h"
#include "backend/Scope.h"
#include "backend/codegen/asm_x86_mem.h"
#include "frontend/syntax/enums.h"

namespace eraxc::JIR {
    class ScopeManager {
        std::vector<Scope> scopes;

        using StackFrame = struct {
            size_t allocationSize;
            size_t topScopeId;
        };

        std::stack<StackFrame> stackFrames;

        using Nodes = std::vector<JIRop>;

    public:
        ScopeManager() {
            //init global scope with default types
            scopes.emplace_back();
            stackFrames.push({0, scopes.size() - 1});
            scopes.back().typenames = std::unordered_map<std::string, size_t> {
                {"i8", syntax::i8},     {"i16", syntax::i16},   {"i32", syntax::i32},
                {"i64", syntax::i64},   {"i128", syntax::i128}, {"i256", syntax::i256},

                {"u8", syntax::u8},     {"u16", syntax::u16},   {"u32", syntax::u32},
                {"u64", syntax::u64},   {"u128", syntax::u128}, {"u256", syntax::u256},

                {"int", syntax::i32},   {"long", syntax::i64},  {"char", syntax::i8},
                {"bool", syntax::BOOL}, {"short", syntax::i16}, {"void", syntax::VOID}};
        }

        u64 addType(const std::string& type) {
            u64 tr = top().typenames.size();
            top().typenames.emplace(type, tr);
            return tr;
        }

        void addTypes(const std::vector<std::string>& typenames) {
            for (const auto& type : typenames) {
                addType(type);
            }
        }

        bool containsTypeRecursive(const std::string& type) const {
            return std::ranges::any_of(scopes, [type](const auto& scope) {
                return scope.typenames.contains(type);
            });
        }

        bool containsType(const std::string& type) const {
            return top().typenames.contains(type);
        }

        bool containsIdRecursive(const std::string& id) const {
            for (const auto& scope : std::ranges::views::reverse(scopes)) {
                if (scope.identifiers.contains(id))
                    return true;
            }
            return false;
        }

        bool containsId(const std::string& id) const {
            return top().identifiers.contains(id);
        }

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

        auto& findDeclarationRecursive(const std::string& name) const {
            for (auto& scope : std::ranges::views::reverse(scopes)) {
                if (auto it = scope.identifiers.find(name); it != scope.identifiers.end()) {
                    return it->second;
                }
            }
            return NOT_FOUND_DECL;
        }

        auto& findDeclaration(const std::string& name) const {
            if (auto it = top().identifiers.find(name); it != top().identifiers.end()) {
                return it->second;
            }
            return NOT_FOUND_DECL;
        }

        void setDeclaration(const std::string& name, const Scope::Declaration& decl) {
            for (auto& scope : std::ranges::views::reverse(scopes)) {
                if (auto it = scope.identifiers.find(name); it != scope.identifiers.end()) {
                    it->second = decl;
                    return;
                }
            }
            // top().identifiers[name] = decl;
        }

        u64 findTypeRecursive(const std::string& type) const {
            for (const auto& scope : std::ranges::views::reverse(scopes)) {
                if (auto it = scope.typenames.find(type); it != scope.typenames.end()) {
                    return it->second;
                }
            }
            return NOT_FOUND;
        }

        u64 findType(const std::string& type) const {
            if (auto it = top().typenames.find(type); it != top().typenames.end())
                return it->second;
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
                nodes.emplace_back(Operation::ALLOC, allocatee, Operand {});
                stackFrames.top().allocationSize += x86::size(type);
                top().allocations.emplace_back(allocatee);
            }
            return top().allocatedIds++;
        }

        //for already allocated ids (e.g. func args)
        size_t addIdWithoutAllocation(const std::string& id, size_t type, bool is_func, bool rValue = false) {
            top().identifiers.emplace(id, Scope::Declaration {type, top().allocatedIds, is_func});
            return top().allocatedIds++;
        }

        size_t addAnonymousId(const u64 type, bool is_func, Nodes& nodes, bool rValue = false) {
            u64& id = top().allocatedIds;
            top().identifiers.emplace("$anonymous" + std::to_string(id), Scope::Declaration {type, id, is_func});
            if (!is_func) {
                Operand allocatee {type, id, false, rValue};
                nodes.emplace_back(Operation::ALLOC, allocatee, Operand {});
                stackFrames.top().allocationSize += x86::size(type);
                top().allocations.emplace_back(allocatee);
            }
            return id++;
        }

        size_t scopesCount() const {
            return scopes.size();
        }

        Scope& top() {
            return scopes.back();
        }
        const Scope& top() const {
            return scopes.back();
        }

        const auto& topFrameSize() const {
            return stackFrames.top().allocationSize;
        }

        void pushFrame() {
            push();
            stackFrames.push({0, scopes.size() - 1});
        }

        void push() {
            scopes.emplace_back();
            if (scopes.size() > 1) {
                scopes.back().allocatedIds = scopes[scopes.size() - 2].allocatedIds;
            }
        }

        void pop(Nodes& nodes) {
            const auto& scope = scopes.back();
            dealloc_scope(nodes, scope);
            scopes.pop_back();
        }

        void popFrame(Nodes& nodes, bool dealloc = true) {
            const auto start_index = stackFrames.top().topScopeId;
            const auto end_index = scopes.size();

            if (dealloc) {
                // [start, end) view
                auto view = scopes | std::views::drop(start_index) | std::views::take(end_index - start_index) |
                    std::views::reverse;
                for (const auto& scope : view) {
                    dealloc_scope(nodes, scope);
                }
            }
            scopes.erase(scopes.begin() + start_index, scopes.end());
            stackFrames.pop();
        }

        void deallocFrame(Nodes& nodes) {
            const auto start_index = stackFrames.top().topScopeId;
            const auto end_index = scopes.size();

            // [start, end) view
            auto view = scopes | std::views::drop(start_index) | std::views::take(end_index - start_index) |
                std::views::reverse;
            for (const auto& scope : view) {
                dealloc_scope(nodes, scope);
            }
        }

    private:
        static void dealloc_scope(Nodes& nodes, const Scope& scope) {
            for (auto& allocatee : std::views::reverse(scope.allocations)) {
                nodes.emplace_back(Operation::DEALLOC, allocatee, Operand {});
            }
        }
    };

}
#pragma once

#include <algorithm>
#include <iostream>
#include <ranges>
#include <stack>

#include "CFG/CFG_parts.h"
#include "Operand.h"
#include "Operation.h"
#include "frontend/syntax/enums.h"

namespace eraxc::JIR {
    class ScopeManager {
        std::vector<Scope> scopes;

        class StackFrame {
            size_t topScopeId;
            size_t currentSize = 0;
            size_t maxSize = 0;

        public:

            explicit StackFrame(size_t topScopeId): topScopeId(topScopeId) {}

            auto getTopScopeId() const {
                return topScopeId;
            }

            auto getMaxSize() const {
                return maxSize;
            }

            void addSize(size_t size) {
                currentSize += size;
                maxSize = std::max(maxSize, currentSize);
            }

            void removeSize(size_t size) {
                currentSize -= size;
            }
        };

        std::stack<StackFrame> stackFrames;

        using Nodes = std::vector<JIRop>;

    public:
        ScopeManager() {
            //init global scope with default types
            scopes.emplace_back(Scope {{},
                                       {{"i8", syntax::i8},
                                        {"i16", syntax::i16},
                                        {"i32", syntax::i32},
                                        {"i64", syntax::i64},
                                        {"i128", syntax::i128},
                                        {"i256", syntax::i256},

                                        {"u8", syntax::u8},
                                        {"u16", syntax::u16},
                                        {"u32", syntax::u32},
                                        {"u64", syntax::u64},
                                        {"u128", syntax::u128},
                                        {"u256", syntax::u256},

                                        {"int", syntax::i32},
                                        {"long", syntax::i64},
                                        {"char", syntax::i8},
                                        {"bool", syntax::BOOL},
                                        {"short", syntax::i16},
                                        {"void", syntax::VOID}}});
            stackFrames.emplace(scopes.size() - 1);
        }

        auto addType(const std::string& type) {
            return top().addType(type);
        }

        void addTypes(const std::vector<std::string>& typenames) {
            for (const auto& type : typenames) {
                addType(type);
            }
        }

        bool containsTypeRecursive(const std::string& type) const {
            return std::ranges::any_of(std::ranges::views::reverse(scopes), [type](const Scope& scope) {
                return scope.containsType(type);
            });
        }

        bool containsIdRecursive(const std::string& id) const {
            return std::ranges::any_of(std::ranges::views::reverse(scopes), [id](const Scope& scope) {
                return scope.containsId(id);
            });
        }

        // for overriding type from higher scope
        bool containsType(const std::string& type) const {
            return top().containsType(type);
        }

        // for overriding id from higher scope
        bool containsId(const std::string& id) const {
            return top().containsId(id);
        }

        static constexpr u64 NOT_FOUND = -1llu;

        std::optional<u64> findIdRecursive(const std::string& id) const {
            for (const auto& scope : std::ranges::views::reverse(scopes)) {
                const auto decl = scope.findId(id);
                if (decl) {
                    return decl.value().getId();
                }
            }
            return std::nullopt;
        }

        std::optional<Scope::Declaration> findDeclarationRecursive(const std::string& name) const {
            for (const auto& scope : std::ranges::views::reverse(scopes)) {
                if (const auto decl = scope.findId(name)) {
                    return decl;
                }
            }
            return std::nullopt;
        }

        std::optional<Scope::Declaration> findDeclaration(const std::string& name) {
            if (const auto decl = top().findId(name)) {
                return decl;
            }
            return std::nullopt;
        }

        void setDeclaration(const std::string& name, const Scope::Declaration& decl) {
            for (auto& scope : std::ranges::views::reverse(scopes)) {
                if (auto id = scope.findId(name)) {
                    id.value() = decl;
                }
            }
        }

        std::optional<u64> findTypeRecursive(const std::string& type) const {
            for (const auto& scope : std::ranges::views::reverse(scopes)) {
                if (const auto opt = scope.findType(type)) {
                    return opt;
                }
            }
            return std::nullopt;
        }

        std::optional<u64> findType(const std::string& type) const {
            if (const auto opt = top().findType(type)) {
                return opt;
            }
            return std::nullopt;
        }

        /// Function to add declaration into scope
        /// \param id declaration to add
        /// \param type id of type of added identifier
        /// \param is_func is this identifier a function
        /// \param nodes nodes list where allocation operation will be added
        /// \return the index of declaration
        std::optional<size_t> addId(const std::string& id, size_t type, bool is_func, Nodes& nodes,
                                    bool rValue = false) {
            const auto opt = top().addId(id, type, is_func);
            if (opt && !is_func) {
                Operand allocatee {type, opt.value().getId(), false, rValue};
                nodes.emplace_back(Operation::ALLOC, allocatee, Operand {});
            }
            if (opt) {
                return opt.value().getId();
            }
            return std::nullopt;
        }

        //for already allocated ids (e.g. func args)
        std::optional<size_t> addIdWithoutAllocation(const std::string& id, size_t type, bool is_func) {
            if (const auto res = top().addId(id, type, is_func, false)) {
                return res.value().getId();
            }
            return std::nullopt;
        }

        size_t addAnonymousId(const u64 type, bool is_func, Nodes& nodes, bool rValue = false) {
            auto& id = top().addAnonymousId(type, is_func);
            if (!is_func) {
                Operand allocatee {type, id.getId(), false, rValue};
                nodes.emplace_back(Operation::ALLOC, allocatee, Operand {});
            }
            return id.getId();
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

        void pushFrame() {
            push();
            stackFrames.emplace(scopes.size() - 1);
        }

        void push() {
            if (scopes.size() > 1) {
                scopes.emplace_back(scopes[scopes.size() - 2].getAllocatedIds());
            } else {
                scopes.emplace_back();
            }
        }

        void pop(CFG_Node& node) {
            auto& scope = scopes.back();
            dealloc_scope(node.body, scope);
            stackFrames.top().addSize(scope.getAllocatedSize());
            node.scope = std::move(scope);
            scopes.pop_back();
        }

        u64 popFrame(CFG_Node& node, bool dealloc = true) {
            const auto start_index = stackFrames.top().getTopScopeId();
            const auto end_index = scopes.size();

            // [start, end) view
            auto view = scopes | std::views::drop(start_index) | std::views::take(end_index - start_index) |
                std::views::reverse;

            for (const auto& scope : view) {
                stackFrames.top().addSize(scope.getAllocatedSize());
            }

            if (dealloc) {
                for (const auto& scope : view) {
                    dealloc_scope(node.body, scope);  //TODO set scope? all scopes collide? what to do
                }
            }
            scopes.erase(scopes.begin() + start_index, scopes.end());
            auto totalSize = stackFrames.top().getMaxSize();
            std::cout << "top frame size: " << totalSize << std::endl;
            stackFrames.pop();
            return totalSize;
        }

        void deallocFrame(Nodes& nodes) {
            const auto start_index = stackFrames.top().getTopScopeId();
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
            // should make calls to dtors
            for (auto& allocatee : scope.getAllocations()) {
                nodes.emplace_back(Operation::DEALLOC, Operand {allocatee.getType(), allocatee.getId(), false, false},
                                   Operand {});
            }
        }
    };
}
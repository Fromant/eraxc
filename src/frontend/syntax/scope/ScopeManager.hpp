#pragma once

#include <functional>
#include <stack>
#include <vector>

#include "Scope.hpp"
#include "common/JIR/Function.hpp"

namespace eraxc::frontend {

    class ScopeManager {
        std::vector<Scope> scopes;

        std::unordered_map<u64, JIR::Function> functions;

        class StackFrame {
            size_t topScopeId;
            size_t currentSize = 0;
            size_t maxSize = 0;

        public:
            explicit StackFrame(size_t topScopeId) : topScopeId(topScopeId) {}

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

    public:
        ScopeManager();

        auto& getFunctions() {
            return functions;
        }

        auto addType(const std::string& type);

        void addTypes(const std::vector<std::string>& typenames);

        bool containsTypeRecursive(const std::string& type) const;

        bool containsIdRecursive(const std::string& id) const;

        // for overriding type from higher scope
        bool containsType(const std::string& type) const;

        // for overriding id from higher scope
        bool containsId(const std::string& id) const;

        static constexpr u64 NOT_FOUND = -1llu;

        std::optional<u64> findIdRecursive(const std::string& id) const;

        std::optional<Scope::Declaration> findDeclarationRecursive(const std::string& name) const;

        std::optional<Scope::Declaration> findDeclaration(const std::string& name);

        void setDeclaration(const std::string& name, const Scope::Declaration& decl);
        void setDeclaration(u64 id, const Scope::Declaration& decl);

        std::optional<u64> findTypeRecursive(const std::string& type) const;

        std::optional<u64> findType(const std::string& type) const;
        std::optional<size_t> addId(const std::string& id, u64 type, bool is_func, bool is_rvalue,
                                    CFG::CFGNode& node);

        /// Function to add declaration into scope
        /// \param name
        /// \param id declaration to add
        /// \param type id of type of added identifier
        /// \param alloc is this identifier a function
        /// \return the index of declaration
        std::optional<size_t> linkId(const std::string& name, u64 id, u64 type, bool alloc);

        //for already allocated ids (e.g. func args)
        std::optional<size_t> addIdWithoutAllocation(const std::string& id, u64 type, bool is_func);
        size_t addAnonymousId(u64 type, bool is_func, bool rValue, CFG::CFGNode& node);

        size_t scopesCount() const;

        Scope& top();
        const Scope& top() const;

        void pushFrame();

        void push();

        error::errable<void> pop();

        error::errable<u64> popFrame();

        error::errable<void> deallocFrame(CFG::CFGNode& node);
    };
}
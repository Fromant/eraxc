#pragma once

#include <algorithm>
#include <iostream>
#include <stack>

#include "CFG/CFG_parts.h"
#include "Operation.h"

namespace eraxc::JIR {
    class ScopeManager {
        std::vector<Scope> scopes;

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

        using Nodes = std::vector<JIRop>;

    public:
        ScopeManager();

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

        std::optional<u64> findTypeRecursive(const std::string& type) const;

        std::optional<u64> findType(const std::string& type) const;

        /// Function to add declaration into scope
        /// \param id declaration to add
        /// \param type id of type of added identifier
        /// \param is_func is this identifier a function
        /// \param nodes nodes list where allocation operation will be added
        /// \return the index of declaration
        std::optional<size_t> addId(const std::string& id, size_t type, bool is_func, CFG_Node& node,
                                    bool rValue = false);

        //for already allocated ids (e.g. func args)
        std::optional<size_t> addIdWithoutAllocation(const std::string& id, size_t type, bool is_func);

        size_t addAnonymousId(const u64 type, bool is_func, CFG_Node& node, bool rValue = false);

        size_t scopesCount() const;

        Scope& top();
        const Scope& top() const;

        void pushFrame();

        void push();

        void pop(CFG_Node& node);

        u64 popFrame(CFG_Node& node, bool dealloc = true);

        void deallocFrame(Nodes& nodes);

    private:
        static void dealloc_scope(Nodes& nodes, const Scope& scope);
    };
}
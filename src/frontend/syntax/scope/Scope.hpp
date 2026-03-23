#pragma once

#include <optional>
#include <ranges>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "frontend/syntax/enums.hpp"
#include "util/common.hpp"

namespace eraxc::frontend {

    class Scope {

    public:
        struct Declaration {
        private:
            u64 type;
            u64 id;
            bool _isFunc;

        public:
            Declaration(u64 type, u64 id, bool is_func) : type(type), id(id), _isFunc(is_func) {}
            Declaration() : type(-1), id(-1), _isFunc(false) {}

            bool isFunc() const {
                return _isFunc;
            }
            u64 getId() const {
                return id;
            }
            u64 getType() const {
                return type;
            }
            error::errable<JIR::Type> getJirType() const {
                const auto t = jirTypeFromKeyword((Keyword)getType());
                if (t == JIR::Type::ERR) {
                    return {"Cannot convert type " + std::to_string(getType()) + " to JIR type", t};
                }
                return {"", t};
            }
            void setId(u64 id) {
                this->id = id;
            }
            bool operator==(const Declaration& declaration) const {
                return type == declaration.type && id == declaration.id && _isFunc == declaration.isFunc();
            };
        };

    private:
        using Type = size_t;
        using TypesMap = std::unordered_map<std::string, Type>;
        using IdentifiersMap = std::unordered_map<std::string, Declaration>;

        // number of allocations (including anonymous)
        u64 allocatedIds = 1;  // reserved 0 for globals allocation function
        // size of all allocations
        u64 allocatedSize = 0;

        IdentifiersMap identifiers {};
        TypesMap typenames {};

        std::vector<Declaration> allocations;

    public:
        Scope() = default;

        explicit Scope(u64 allocatedIds) : allocatedIds(allocatedIds) {}

        Scope(const IdentifiersMap& identifiers, TypesMap typenames) :
            allocatedIds(identifiers.size() + 1), identifiers(identifiers), typenames(std::move(typenames)) {}

        Scope(const Scope& other) {
            allocatedIds = other.allocatedIds;
            identifiers = other.identifiers;
            typenames = other.typenames;
            allocatedSize = other.allocatedSize;
            allocations = other.allocations;
        }

        Scope(Scope&& other) noexcept {
            allocatedIds = other.allocatedIds;
            identifiers.swap(other.identifiers);
            typenames.swap(other.typenames);
            allocatedSize = other.allocatedSize;
            allocations.swap(other.allocations);
            other.allocatedIds = 0;
            other.allocatedSize = 0;
        }

        Scope& operator=(Scope&& other) noexcept {
            if (&other == this) {
                return *this;
            }
            allocatedIds = other.allocatedIds;
            identifiers.swap(other.identifiers);
            typenames.swap(other.typenames);
            allocatedSize = other.allocatedSize;
            allocations.swap(other.allocations);
            other.allocatedIds = 0;
            other.allocatedSize = 0;
            return *this;
        }

        Scope& operator=(const Scope& other) noexcept {
            if (&other == this) {
                return *this;
            }
            allocatedIds = other.allocatedIds;
            identifiers = other.identifiers;
            typenames = other.typenames;
            allocatedSize = other.allocatedSize;
            allocations = other.allocations;
            return *this;
        }

        u64 getAllocatedIds() const {
            return allocatedIds;
        }

        u64 getAllocatedSize() const {
            return allocatedSize;
        }

        bool containsId(const std::string& id) const {
            return identifiers.contains(id);
        }

        bool containsType(const std::string& type) const {
            return typenames.contains(type);
        }

        auto getIdentifiers() const {
            return std::views::all(identifiers);
        }

        auto getTypenames() const {
            return std::views::all(typenames);
        }

        // already reversed
        auto getAllocations() const {
            return std::views::all(allocations) | std::views::reverse;
        }

        std::optional<Declaration> findId(const std::string& id) const {
            const auto it = identifiers.find(id);
            if (it != identifiers.end()) {
                return it->second;
            }
            return std::nullopt;
        }

        bool setDeclaration(const std::string& id, const Declaration& decl) {
            if (const auto it = identifiers.find(id); it != identifiers.end()) {
                it->second = decl;
                return true;
            }
            return false;
        }

        bool setDeclaration(u64 id, const Declaration& decl) {
            const auto it = std::ranges::find_if(identifiers, [&](const auto& p) {
                return p.second.getId() == id;
            });
            if (it != identifiers.end()) {
                it->second = decl;
                return true;
            }
            return false;
        }

        std::optional<size_t> findType(const std::string& type) const {
            const auto it = typenames.find(type);
            if (it != typenames.end()) {
                return it->second;
            }
            return std::nullopt;
        }

        std::optional<Declaration> addId(const std::string& id, u64 type, bool isFunc, bool doAllocate = true) {
            auto r = identifiers.emplace(id, Declaration {type, allocatedIds++, isFunc});
            if (!r.second) {
                return std::nullopt;
            }
            if (doAllocate && !isFunc) {
                const auto size_opt = type_size((Keyword)type);
                if (!size_opt) {
                    return std::nullopt;
                }
                allocatedSize += size_opt.value;
                allocations.emplace_back(r.first->second);
            }
            return r.first->second;
        }

        std::optional<Declaration> addId(const std::string& name, u64 id, u64 type, bool doAllocate = false) {
            auto r = identifiers.emplace(name, Declaration {type, id, false});
            if (!r.second) {
                return std::nullopt;
            }
            if (doAllocate) {
                const auto size_opt = type_size((Keyword)type);
                if (!size_opt) {
                    return std::nullopt;
                }
                allocatedSize += size_opt.value;
                allocations.emplace_back(r.first->second);
            }
            return r.first->second;
        }

        Declaration& addAnonymousId(u64 type, bool isFunc, bool doAllocate = true) {
            const auto name = "$anonimous" + std::to_string(allocatedIds);
            auto r = identifiers.emplace(name, Declaration {type, allocatedIds++, isFunc});
            if (!r.second) {
                throw std::runtime_error("cannot add " + name + " to scope. Is it already allocated?");
            }
            if (doAllocate && !isFunc) {
                if (const auto size_opt = type_size((Keyword)type)) {
                    allocatedSize += size_opt.value;
                }
                allocations.emplace_back(r.first->second);
            }
            return r.first->second;
        }

        std::optional<u64> addType(const std::string& type) {
            auto r = typenames.emplace(type, typenames.size());
            if (!r.second) {
                return std::nullopt;
            }
            return r.first->second;
        }
    };
}

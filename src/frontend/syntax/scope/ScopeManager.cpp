#include "ScopeManager.hpp"

#include <algorithm>
#include <iostream>

#include "frontend/syntax/parser/ExpressionParser.hpp"

using namespace eraxc::frontend;

void dealloc_scope(std::vector<eraxc::JIR::Command>& tokens, const Scope& scope) {
    for (const auto& allocated : scope.getAllocations()) {
        tokens.emplace_back(eraxc::JIR::Command {
            eraxc::JIR::Operation::DEALLOC,
            eraxc::JIR::Operand {(eraxc::JIR::Type)allocated.getId(), allocated.getType(), false, false},
            eraxc::JIR::Operand {}});
    }
}


ScopeManager::ScopeManager() {
    //init global scope with default types
    scopes.emplace_back(Scope {{},
                               {{"i8", (size_t)Keyword::i8},
                                {"i16", (size_t)Keyword::i16},
                                {"i32", (size_t)Keyword::i32},
                                {"i64", (size_t)Keyword::i64},
                                {"i128", (size_t)Keyword::i128},
                                {"i256", (size_t)Keyword::i256},

                                {"u8", (size_t)Keyword::u8},
                                {"u16", (size_t)Keyword::u16},
                                {"u32", (size_t)Keyword::u32},
                                {"u64", (size_t)Keyword::u64},
                                {"u128", (size_t)Keyword::u128},
                                {"u256", (size_t)Keyword::u256},

                                {"int", (size_t)Keyword::i32},
                                {"long", (size_t)Keyword::i64},
                                {"char", (size_t)Keyword::i8},
                                {"bool", (size_t)Keyword::BOOL},
                                {"short", (size_t)Keyword::i16},
                                {"void", (size_t)Keyword::VOID}}});
    stackFrames.emplace(scopes.size() - 1);
}

auto ScopeManager::addType(const std::string& type) {
    return top().addType(type);
}

void ScopeManager::addTypes(const std::vector<std::string>& typenames) {
    for (const auto& type : typenames) {
        addType(type);
    }
}

bool ScopeManager::containsTypeRecursive(const std::string& type) const {
    return std::ranges::any_of(std::ranges::views::reverse(scopes), [type](const Scope& scope) {
        return scope.containsType(type);
    });
}

bool ScopeManager::containsIdRecursive(const std::string& id) const {
    return std::ranges::any_of(std::ranges::views::reverse(scopes), [id](const Scope& scope) {
        return scope.containsId(id);
    });
}

bool ScopeManager::containsType(const std::string& type) const {
    return top().containsType(type);
}

bool ScopeManager::containsId(const std::string& id) const {
    return top().containsId(id);
}

std::optional<u64> ScopeManager::findIdRecursive(const std::string& id) const {
    for (const auto& scope : std::ranges::views::reverse(scopes)) {
        if (const auto decl = scope.findId(id)) {
            return decl.value().getId();
        }
    }
    return std::nullopt;
}

std::optional<Scope::Declaration> ScopeManager::findDeclarationRecursive(const std::string& name) const {
    for (const auto& scope : std::ranges::views::reverse(scopes)) {
        if (const auto decl = scope.findId(name)) {
            return decl;
        }
    }
    return std::nullopt;
}

std::optional<Scope::Declaration> ScopeManager::findDeclaration(const std::string& name) {
    if (const auto decl = top().findId(name)) {
        return decl;
    }
    return std::nullopt;
}

void ScopeManager::setDeclaration(const std::string& name, const Scope::Declaration& decl) {
    for (auto& scope : std::ranges::views::reverse(scopes)) {
        if (scope.setDeclaration(name, decl)) {
            return;
        }
    }
}

void ScopeManager::setDeclaration(u64 id, const Scope::Declaration& decl) {
    for (auto& scope : std::ranges::views::reverse(scopes)) {
        if (scope.setDeclaration(id, decl)) {
            return;
        }
    }
}

std::optional<u64> ScopeManager::findTypeRecursive(const std::string& type) const {
    for (const auto& scope : std::ranges::views::reverse(scopes)) {
        if (const auto opt = scope.findType(type)) {
            return opt;
        }
    }
    return std::nullopt;
}

std::optional<u64> ScopeManager::findType(const std::string& type) const {
    if (const auto opt = top().findType(type)) {
        return opt;
    }
    return std::nullopt;
}

std::optional<size_t> ScopeManager::addId(const std::string& id, size_t type, bool is_func, bool is_rvalue) {
    if (const auto opt = top().addId(id, type, is_func)) {
        return opt.value().getId();
    }
    return std::nullopt;
}

std::optional<size_t> ScopeManager::addIdWithoutAllocation(const std::string& id, size_t type, bool is_func) {
    if (const auto res = top().addId(id, type, is_func, false)) {
        return res.value().getId();
    }
    return std::nullopt;
}

size_t ScopeManager::addAnonymousId(const u64 type, bool is_func, bool rValue) {
    auto& id = top().addAnonymousId(type, is_func);
    return id.getId();
}

size_t ScopeManager::scopesCount() const {
    return scopes.size();
}

Scope& ScopeManager::top() {
    return scopes.back();
}

const Scope& ScopeManager::top() const {
    return scopes.back();
}

void ScopeManager::pushFrame() {
    push();
    stackFrames.emplace(scopes.size() - 1);
}

void ScopeManager::push() {
    if (!scopes.empty()) {
        scopes.emplace_back(scopes[scopes.size() - 1].getAllocatedIds());
    } else {
        scopes.emplace_back();
    }
}

void ScopeManager::pop(std::vector<JIR::Command>& tokens) {
    auto& scope = scopes.back();
    dealloc_scope(tokens, scope);
    stackFrames.top().addSize(scope.getAllocatedSize());
    scopes.pop_back();
}

u64 ScopeManager::popFrame() {
    const auto start_index = stackFrames.top().getTopScopeId();
    const auto end_index = scopes.size();

    // [start, end) view
    auto view =
        scopes | std::views::drop(start_index) | std::views::take(end_index - start_index) | std::views::reverse;

    for (const auto& scope : view) {
        stackFrames.top().addSize(scope.getAllocatedSize());
    }
    scopes.erase(scopes.begin() + start_index, scopes.end());
    auto totalSize = stackFrames.top().getMaxSize();
    std::cout << "top frame size: " << totalSize << std::endl;
    stackFrames.pop();
    return totalSize;
}

void ScopeManager::deallocFrame(std::vector<JIR::Command>& tokens) {
    const auto start_index = stackFrames.top().getTopScopeId();
    const auto end_index = scopes.size();

    // [start, end) view
    auto view =
        scopes | std::views::drop(start_index) | std::views::take(end_index - start_index) | std::views::reverse;
    for (const auto& scope : view) {
        dealloc_scope(tokens, scope);
    }
}

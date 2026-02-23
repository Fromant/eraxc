#include "ScopeManager.h"

using namespace eraxc::JIR;

ScopeManager::ScopeManager() {
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
        const auto decl = scope.findId(id);
        if (decl) {
            return decl.value().getId();
        }
    }
    return std::nullopt;
}

std::optional<eraxc::Scope::Declaration> ScopeManager::findDeclarationRecursive(const std::string& name) const {
    for (const auto& scope : std::ranges::views::reverse(scopes)) {
        if (const auto decl = scope.findId(name)) {
            return decl;
        }
    }
    return std::nullopt;
}

std::optional<eraxc::Scope::Declaration> ScopeManager::findDeclaration(const std::string& name) {
    if (const auto decl = top().findId(name)) {
        return decl;
    }
    return std::nullopt;
}

void ScopeManager::setDeclaration(const std::string& name, const Scope::Declaration& decl) {
    for (auto& scope : std::ranges::views::reverse(scopes)) {
        if (auto id = scope.findId(name)) {
            id.value() = decl;
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

std::optional<size_t> ScopeManager::addId(const std::string& id, size_t type, bool is_func, CFG_Node& node, bool rValue) {
    const auto opt = top().addId(id, type, is_func);
    if (opt && !is_func) {
        Operand allocatee {type, opt.value().getId(), false, rValue};
        node.body.emplace_back(Operation::ALLOC, allocatee, Operand {});
        node.allocatedIds.emplace_back(type, opt.value().getId());
    }
    if (opt) {
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

size_t ScopeManager::addAnonymousId(const u64 type, bool is_func, CFG_Node& node, bool rValue) {
    auto& id = top().addAnonymousId(type, is_func);
    if (!is_func) {
        Operand allocatee {type, id.getId(), false, rValue};
        node.body.emplace_back(Operation::ALLOC, allocatee, Operand {});
        node.allocatedIds.emplace_back(type, id.getId());
    }
    return id.getId();
}

size_t ScopeManager::scopesCount() const {
    return scopes.size();
}

eraxc::Scope& ScopeManager::top() {
    return scopes.back();
}

const eraxc::Scope& ScopeManager::top() const {
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

void ScopeManager::pop(CFG_Node& node) {
    auto& scope = scopes.back();
    dealloc_scope(node.body, scope);
    stackFrames.top().addSize(scope.getAllocatedSize());
    scopes.pop_back();
}

u64 ScopeManager::popFrame(CFG_Node& node, bool dealloc) {
    const auto start_index = stackFrames.top().getTopScopeId();
    const auto end_index = scopes.size();

    // [start, end) view
    auto view =
        scopes | std::views::drop(start_index) | std::views::take(end_index - start_index) | std::views::reverse;

    for (const auto& scope : view) {
        stackFrames.top().addSize(scope.getAllocatedSize());
    }

    if (dealloc) {
        for (auto& scope : view) {
            dealloc_scope(node.body, scope);
        }
    }
    scopes.erase(scopes.begin() + start_index, scopes.end());
    auto totalSize = stackFrames.top().getMaxSize();
    std::cout << "top frame size: " << totalSize << std::endl;
    stackFrames.pop();
    return totalSize;
}

void ScopeManager::deallocFrame(Nodes& nodes) {
    const auto start_index = stackFrames.top().getTopScopeId();
    const auto end_index = scopes.size();

    // [start, end) view
    auto view =
        scopes | std::views::drop(start_index) | std::views::take(end_index - start_index) | std::views::reverse;
    for (const auto& scope : view) {
        dealloc_scope(nodes, scope);
    }
}

void ScopeManager::dealloc_scope(Nodes& nodes, const Scope& scope) {
    // should make calls to dtors
    for (auto& allocatee : scope.getAllocations()) {
        nodes.emplace_back(Operation::DEALLOC, Operand {allocatee.getType(), allocatee.getId(), false, false},
                           Operand {});
    }
}
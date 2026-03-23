#pragma once

#include <unordered_map>

#include "Declaration.hpp"
#include "Function.hpp"

namespace eraxc::JIR {
    struct SymbolTable {
        using FunctionsMap = std::unordered_map<std::string, Function>;
        using globalsMap = std::unordered_map<std::string, Declaration>;
    };
}

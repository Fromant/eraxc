#pragma once

#include <vector>

#include "Function.hpp"
#include "Global.hpp"
#include "SymbolTable.hpp"

namespace eraxc::JIR {
    struct FrontendResult {
        std::vector<Global> globals;
        std::vector<Function> functions;

        SymbolTable symbols;
        // module imports
        // module exports

        void print() const;
    };
}
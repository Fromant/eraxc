#pragma once

#include <vector>

#include "Function.hpp"
#include "Global.hpp"
#include "SymbolTable.hpp"

namespace eraxc::JIR {
    struct FrontendResult {
        std::vector<Global> globals;
        std::vector<Function> functions;

        size_t entrypointId;

        SymbolTable symbols;
        // module imports
        // module exports

        void print() const;
        void print_to_file(const std::string& str) const;
    };
}
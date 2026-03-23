#pragma once
#include "Declaration.hpp"
#include "common/CFG/CFGParts.hpp"

namespace eraxc::JIR {
    class Global {
    public:
        Declaration decl;
        // needs CFG because ternary operator and other expression-compatible ways of branching code exists
        CFG::CFG init_cfg;
    };
}
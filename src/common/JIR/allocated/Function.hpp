#pragma once

#include <vector>

#include "common/CFG/allocated/AllocatedCFG.hpp"
#include "common/JIR/Declaration.hpp"

namespace eraxc::JIR::allocated {
    class Function {
    public:
        Declaration decl;
        std::vector<Declaration> params;
        CFG::allocated::CFG cfg;

        Function() : params({}), cfg({}) {}
    };
}
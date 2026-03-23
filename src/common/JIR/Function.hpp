#pragma once
#include <vector>

#include "Declaration.hpp"
#include "common/CFG/CFGParts.hpp"

namespace eraxc::JIR {

    class Function {
    public:
        Declaration decl;
        std::vector<Declaration> params;
        CFG::CFG cfg;

        Function() : params({}), cfg({}) {}
    };

}

#pragma once

#include "Function.hpp"
#include "Global.hpp"

namespace eraxc::JIR {
    class Program {
    public:
        std::vector<Function> functions;
        std::vector<Global> globals;
        Program(const std::vector<Function>& functions, const std::vector<Global>& globals) :
            functions(functions), globals(globals) {};

        Program(const Program& program) = default;
    };
}

#pragma once
#include "Function.hpp"
#include "common/JIR/Global.hpp"

namespace eraxc::JIR::allocated {
    class Program {
    public:
        std::vector<Function> functions;
        std::vector<Global> globals;
        Program(const std::vector<Function>& functions, const std::vector<Global>& globals) :
            functions(functions), globals(globals) {};

        Program& operator=(const Program& program) = default;
    };
}

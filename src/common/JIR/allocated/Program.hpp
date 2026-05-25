#pragma once

#include "Function.hpp"
#include "common/JIR/Global.hpp"

namespace eraxc::JIR::allocated {
    class Program {
    public:
        std::vector<Function> functions;
        std::vector<Global> globals;
        size_t entrypoint_id;
        Program(const std::vector<Function>& functions, const std::vector<Global>& globals, size_t entrypoint_id) :
            functions(functions), globals(globals), entrypoint_id(entrypoint_id) {};

        Program& operator=(const Program& program) = default;
    };
}

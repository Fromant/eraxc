#pragma once

#include "Function.hpp"
#include "Global.hpp"

namespace eraxc::JIR {
    class Program {
    public:
        std::vector<Function> functions;
        size_t entrypoint_id;
        std::vector<Global> globals;
        Program(const std::vector<Function>& functions, const std::vector<Global>& globals, size_t entrypoint_id) :
            functions(functions), globals(globals), entrypoint_id(entrypoint_id) {};

        Program(const Program& program) = default;
    };
}

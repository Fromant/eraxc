#pragma once
#include <vector>

#include "common/CFG/CFGParts.hpp"

namespace eraxc::JIR {

    // TODO раскидать это все по файлам
    struct Declaration {
        u64 id;
        Type type;

        Declaration(const Operand& op) {
            id = op.value;
            type = op.type;
        }

        Declaration(u64 id, Type type) : id(id), type(type) {}

        Declaration() {
            id = -1;
            type = Type::ERR;
        }
    };

    class Function {
    public:
        Declaration decl;
        std::vector<Declaration> params;
        CFG::CFG cfg;

        Function() : params({}), cfg({}) {}
    };

    class Global {
    public:
        Declaration decl;
        // needs CFG because ternary operator and other expression-compatible ways of branching code exists
        CFG::CFG init_cfg;
    };

    struct SymbolTable {
        using FunctionsMap = std::unordered_map<std::string, Function>;
        using globalsMap = std::unordered_map<std::string, Declaration>;
    };

    struct FrontendResult {
        std::vector<Global> globals;
        std::vector<Function> functions;

        SymbolTable symbols;
        // module imports
        // module exports

        void print() const;
    };
}

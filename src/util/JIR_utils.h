//
// Created by frama on 29.01.2025.
//

#ifndef BLCK_COMPILER_IL_UTILS_H
#define BLCK_COMPILER_IL_UTILS_H

#include <iostream>

#include "../backend/JIR_handler.h"

namespace eraxc::JIR {
    inline std::string operand_to_string(const JIR_operand& op) {
        std::string result {};
        if (!op.is_const) result += '$';
        result += std::to_string(op.value);
        return result;
    }

    inline void print_JIR_nodes(const std::vector<JIR_node>& nodes) {
        for (const auto& node : nodes) {
            if (node.operation == JIR_op::MOVE) {
                std::cout << "MOVE " << operand_to_string(node.operand1) << ' ' << operand_to_string(node.operand2)
                          << std::endl;
            } else if (node.operation == JIR_op::ADD) {
                std::cout << "ADD " << operand_to_string(node.operand1) << ' ' << operand_to_string(node.operand2)
                          << std::endl;
            } else if (node.operation == JIR_op::SUB) {
                std::cout << "SUB " << operand_to_string(node.operand1) << ' ' << operand_to_string(node.operand2)
                          << std::endl;
            } else if (node.operation == JIR_op::MUL) {
                std::cout << "MUL " << operand_to_string(node.operand1) << ' ' << operand_to_string(node.operand2)
                          << std::endl;
            } else if (node.operation == JIR_op::DIV) {
                std::cout << "DIV " << operand_to_string(node.operand1) << ' ' << operand_to_string(node.operand2)
                          << std::endl;
            } else if (node.operation == JIR_op::RET) {
                std::cout << "RET " << operand_to_string(node.operand1) << std::endl;
            } else if (node.operation == JIR_op::MOD) {
                std::cout << "MOD " << operand_to_string(node.operand1) << ' ' << operand_to_string(node.operand2)
                          << std::endl;
            } else if (node.operation == JIR_op::NOT) {
                std::cout << "NOT " << operand_to_string(node.operand1) << std::endl;
            } else if (node.operation == JIR_op::NEG) {
                std::cout << "NEG " << operand_to_string(node.operand1) << std::endl;
            } else if (node.operation == JIR_op::AND) {
                std::cout << "AND " << operand_to_string(node.operand1) << ' ' << operand_to_string(node.operand2)
                          << std::endl;
            } else if (node.operation == JIR_op::OR) {
                std::cout << "OR " << operand_to_string(node.operand1) << ' ' << operand_to_string(node.operand2)
                          << std::endl;
            } else if (node.operation == JIR_op::XOR) {
                std::cout << "XOR " << operand_to_string(node.operand1) << ' ' << operand_to_string(node.operand2)
                          << std::endl;
            } else if (node.operation == JIR_op::LSHIFT) {
                std::cout << "LSHIFT " << operand_to_string(node.operand1) << ' ' << operand_to_string(node.operand2)
                          << std::endl;
            } else if (node.operation == JIR_op::RSHIFT) {
                std::cout << "RSHIFT " << operand_to_string(node.operand1) << ' ' << operand_to_string(node.operand2)
                          << std::endl;
            } else if (node.operation == JIR_op::CMP) {
                std::cout << "CMP " << operand_to_string(node.operand1) << ' ' << operand_to_string(node.operand2)
                          << std::endl;
            } else if (node.operation == JIR_op::LABEL) {
                std::cout << ".l" << node.operand1.value << ':' << std::endl;
            }else if (node.operation == JIR_op::JUMP) {
                std::cout << "JMP .l" << node.operand1.value << std::endl;
            } else if (node.operation == JIR_op::JE) {
                std::cout << "JE .l" << node.operand1.value << std::endl;
            } else if (node.operation == JIR_op::JNE) {
                std::cout << "JNE .l" << node.operand1.value << std::endl;
            } else if (node.operation == JIR_op::JL) {
                std::cout << "JL .l" << node.operand1.value << std::endl;
            } else if (node.operation == JIR_op::JLE) {
                std::cout << "JLE .l" << node.operand1.value << std::endl;
            } else if (node.operation == JIR_op::JG) {
                std::cout << "JG .l" << node.operand1.value << std::endl;
            } else if (node.operation == JIR_op::JGE) {
                std::cout << "JGE .l" << node.operand1.value << std::endl;
            } else if (node.operation == JIR_op::JUMP) {
                std::cout << "JUMP $f_ " << operand_to_string(node.operand1) << std::endl;
            } else if (node.operation == JIR_op::CALL) {
                std::cout << operand_to_string(node.operand2) << " = CALL $f_" << operand_to_string(node.operand1)
                          << std::endl;
            } else if (node.operation == JIR_op::PASS) {
                std::cout << "PASS " << node.operand1.type << " " << operand_to_string(node.operand1) << std::endl;
            } else if (node.operation == JIR_op::INC) {
                std::cout << "INC " << operand_to_string(node.operand1) << std::endl;
            } else if (node.operation == JIR_op::DEC) {
                std::cout << "DEC " << operand_to_string(node.operand1) << std::endl;
            } else if (node.operation == JIR_op::ALLOC) {
                std::cout << "ALLOC " << operand_to_string(node.operand1) << std::endl;
            }
        }
    }

    inline void print_JIR_funcs(const std::unordered_map<u64, JIR_function>& funcs) {
        for (const auto& func : funcs) {
            std::cout << func.second.return_type << " $" << func.second.id << " (";
            const size_t args_size = func.second.args.size();
            if (args_size > 0) {
                for (int i = 0; i < args_size - 1; i++) {
                    const auto& decl = func.second.args[i];
                    std::cout << decl.type << " $" << decl.value << ", ";
                }
                std::cout << func.second.args[args_size - 1].type << " $" << func.second.args[args_size - 1].value
                          << ") {\n";
            } else std::cout << ") {\n";

            print_JIR_nodes(func.second.body);

            std::cout << "}\n";
        }
    }
}

#endif  //BLCK_COMPILER_IL_UTILS_H

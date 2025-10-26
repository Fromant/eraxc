#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
#include <vector>

#include "Node.h"
#include "Operand.h"
#include "CFG/CFG_parts.h"

namespace eraxc::JIR::utils {

    inline std::string int_to_hex(int i) {
        std::stringstream ss;
        ss << std::hex << i;
        return ss.str();
    }

    inline std::string operand_to_string(const Operand& op) {
        std::string result {};
        if (!op.is_instant) result += '$';
        result += std::to_string(op.value);
        return result;
    }

    inline void print_JIR_nodes(const std::vector<Node>& nodes) {
        for (const auto& node : nodes) {
            if (node.op == Operation::MOVE) {
                std::cout << "MOVE " << operand_to_string(node.operand1) << ' ' << operand_to_string(node.operand2)
                          << std::endl;
            } else if (node.op == Operation::ADD) {
                std::cout << "ADD " << operand_to_string(node.operand1) << ' ' << operand_to_string(node.operand2)
                          << std::endl;
            } else if (node.op == Operation::SUB) {
                std::cout << "SUB " << operand_to_string(node.operand1) << ' ' << operand_to_string(node.operand2)
                          << std::endl;
            } else if (node.op == Operation::MUL) {
                std::cout << "MUL " << operand_to_string(node.operand1) << ' ' << operand_to_string(node.operand2)
                          << std::endl;
            } else if (node.op == Operation::DIV) {
                std::cout << "DIV " << operand_to_string(node.operand1) << ' ' << operand_to_string(node.operand2)
                          << std::endl;
            } else if (node.op == Operation::RET) {
                std::cout << "RET" << std::endl;
            } else if (node.op == Operation::MOD) {
                std::cout << "MOD " << operand_to_string(node.operand1) << ' ' << operand_to_string(node.operand2)
                          << std::endl;
            } else if (node.op == Operation::NOT) {
                std::cout << "NOT " << operand_to_string(node.operand1) << std::endl;
            } else if (node.op == Operation::NEG) {
                std::cout << "NEG " << operand_to_string(node.operand1) << std::endl;
            } else if (node.op == Operation::AND) {
                std::cout << "AND " << operand_to_string(node.operand1) << ' ' << operand_to_string(node.operand2)
                          << std::endl;
            } else if (node.op == Operation::OR) {
                std::cout << "OR " << operand_to_string(node.operand1) << ' ' << operand_to_string(node.operand2)
                          << std::endl;
            } else if (node.op == Operation::XOR) {
                std::cout << "XOR " << operand_to_string(node.operand1) << ' ' << operand_to_string(node.operand2)
                          << std::endl;
            } else if (node.op == Operation::LSHIFT) {
                std::cout << "LSHIFT " << operand_to_string(node.operand1) << ' ' << operand_to_string(node.operand2)
                          << std::endl;
            } else if (node.op == Operation::RSHIFT) {
                std::cout << "RSHIFT " << operand_to_string(node.operand1) << ' ' << operand_to_string(node.operand2)
                          << std::endl;
            } else if (node.op == Operation::CMP) {
                std::cout << "CMP " << operand_to_string(node.operand1) << ' ' << operand_to_string(node.operand2)
                          << std::endl;
            } else if (node.op == Operation::LABEL) {
                std::cout << ".l" << node.operand1.value << ':' << std::endl;
            } else if (node.op == Operation::JUMP) {
                std::cout << "JMP .l" << node.operand1.value << std::endl;
            } else if (node.op == Operation::JE) {
                std::cout << "JE .l" << node.operand1.value << std::endl;
            } else if (node.op == Operation::JNE) {
                std::cout << "JNE .l" << node.operand1.value << std::endl;
            } else if (node.op == Operation::JL) {
                std::cout << "JL .l" << node.operand1.value << std::endl;
            } else if (node.op == Operation::JLE) {
                std::cout << "JLE .l" << node.operand1.value << std::endl;
            } else if (node.op == Operation::JG) {
                std::cout << "JG .l" << node.operand1.value << std::endl;
            } else if (node.op == Operation::JGE) {
                std::cout << "JGE .l" << node.operand1.value << std::endl;
            } else if (node.op == Operation::JUMP) {
                std::cout << "JUMP $f_ " << operand_to_string(node.operand1) << std::endl;
            } else if (node.op == Operation::CALL) {
                std::cout << operand_to_string(node.operand2) << " = CALL $f_" << operand_to_string(node.operand1)
                          << std::endl;
            } else if (node.op == Operation::PASS) {
                std::cout << "PASS " << node.operand1.type << " " << operand_to_string(node.operand1) << std::endl;
            } else if (node.op == Operation::INC) {
                std::cout << "INC " << operand_to_string(node.operand1) << std::endl;
            } else if (node.op == Operation::DEC) {
                std::cout << "DEC " << operand_to_string(node.operand1) << std::endl;
            } else if (node.op == Operation::ALLOC) {
                std::cout << "ALLOC " << operand_to_string(node.operand1) << std::endl;
            } else if (node.op == Operation::DEALLOC) {
                std::cout << "DEALLOC " << operand_to_string(node.operand1) << std::endl;
            }
        }
    }

    inline void print_CFG_nodes(const std::unordered_map<u64, CFG_Node>& nodes) {
        for (const auto& node : nodes) {
            // std::cout << node.second. << " $" << func.second.id << " (";
            // const size_t args_size = func.second.args.size();
            // if (args_size > 0) {
            //     for (int i = 0; i < args_size - 1; i++) {
            //         const auto& decl = func.second.args[i];
            //         std::cout << decl.type << " $" << decl.value << ", ";
            //     }
            //     std::cout << func.second.args[args_size - 1].type << " $" << func.second.args[args_size - 1].value
            //               << ") {\n";
            // } else std::cout << ") {\n";
            //
            // print_JIR_nodes(func.second.body);
            //
            // std::cout << "}\n";
        }
    }
}

#endif //UTILS_H

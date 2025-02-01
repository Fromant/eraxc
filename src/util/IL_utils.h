//
// Created by frama on 29.01.2025.
//

#ifndef BLCK_COMPILER_IL_UTILS_H
#define BLCK_COMPILER_IL_UTILS_H

namespace eraxc::IL {

    std::string operand_to_string(const IL_operand &op) {
        std::string result{};
        if (!op.is_instant) result += '$';
        result += std::to_string(op.id);
        return result;
    }

    void print_IL_nodes(const std::vector<IL_node> &nodes) {
        for (const auto &node: nodes) {
            if (node.op == IL_node::ASSIGN) {
                std::cout << node.assignee_type << ' ' << '$' << node.assignee << '='
                          << operand_to_string(node.operand1) << std::endl;
            } else if (node.op == IL_node::ADD) {
                std::cout << node.assignee_type << ' ' << '$' << node.assignee << '='
                          << operand_to_string(node.operand1) << '+'
                          << operand_to_string(node.operand2) << std::endl;
            } else if (node.op == IL_node::SUB) {
                std::cout << node.assignee_type << ' ' << '$' << node.assignee << '='
                          << operand_to_string(node.operand1) << '-'
                          << operand_to_string(node.operand2) << std::endl;

            }
        }
    }

    void print_IL_funcs(const std::unordered_map<u64, IL_func> &funcs) {
        for (const auto &func: funcs) {
            std::cout << func.second.declaration.type << " $" << func.first << " (";
            const size_t args_size = func.second.args.size();
            if (args_size > 0) {
                for (int i = 0; i < args_size - 1; i++) {
                    const auto &decl = func.second.args[i];
                    std::cout << decl.type << " $" << decl.id << ", ";
                }
                std::cout << func.second.args[args_size - 1].type << " $" << func.second.args[args_size - 1].id
                          << ") {\n";
            } else std::cout << ") {\n";

            print_IL_nodes(func.second.body);

            std::cout << "}\n";
        }
    }
}

#endif //BLCK_COMPILER_IL_UTILS_H

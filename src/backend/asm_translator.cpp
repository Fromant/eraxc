#include <fstream>
#include "asm_translator.h"


namespace eraxc::backend {


    std::string asm_translator::translate_NASM(const std::vector<syntax::AST::node> &tree,
                                               const std::string &o_filename) {
        std::stringstream init_globals{};

        std::ofstream of{o_filename};

        std::vector<size_t> total_global_consts{};

        of << "global main\nbits 64\nextern printf\nsection .data\n";
        //generate .text

        for (const auto &node: tree) {
            if (node.type == syntax::AST::node::DECLARATION) {
                const auto &decl = node.d.decl;
                of << "u64_" << decl.id << " dq 0\n";

                auto it = &decl.assign_to;
                init_globals << "xor rax, rax\n";
                while (it->right != nullptr) {
                    //one-by-one very slow initialization
                    if (it->op == syntax::ASSIGN) {
                        if (it->isInstant) init_globals << "mov rax, " << it->data.data << '\n';
                        else init_globals << "mov rax, [rel u64_" << it->data.data << "]\n";
                    } else if (it->op == syntax::ADD) {
                        if (it->isInstant) init_globals << "add rax, " << it->data.data << '\n';
                        else init_globals << "add rax, [rel u64_" << it->data.data << "]\n";
                    } else if (it->op == syntax::MULTIPLY) {
                        if (it->isInstant) init_globals << "imul rax, " << it->data.data << '\n';
                        else init_globals << "imul rax, [rel u64_" << it->data.data << "]\n";
                    }
                    it = it->right;
                }
                if (it->op == syntax::ASSIGN) {
                    if (it->isInstant) init_globals << "mov rax, " << it->data.data << '\n';
                    else init_globals << "mov rax, [rel u64_" << it->data.data << "]\n";
                } else if (it->op == syntax::ADD) {
                    if (it->isInstant) init_globals << "add rax, " << it->data.data << '\n';
                    else init_globals << "add rax, [rel u64_" << it->data.data << "]\n";
                } else if (it->op == syntax::MULTIPLY) {
                    if (it->isInstant) init_globals << "imul rax, " << it->data.data << '\n';
                    else init_globals << "imul rax, [rel u64_" << it->data.data << "]\n";
                }

                //save value
                init_globals << "mov QWORD [rel u64_" << decl.id << "], rax\n";
                total_global_consts.emplace_back(decl.id);
            }
        }

        of << "DBG_PRINT: db \"{%d}: %d\", 0x0A, 0x00\n";

        //.text
        of << "section .text\nmain:\nsub rsp, 0x28\n";
        of << "call init_global_scope\n";

        //there goes main() body

        of << "add rsp, 0x28\nxor rax, rax\nret;exit(0)\n"; //return 0

        of << "\ninit_global_scope:\nsub rsp, 0x08\n";
        of << init_globals.str();
        of << "call debug_print_global_scope\n";
        of << "add rsp, 0x08\nret\n";

        of << "\ndebug_print_global_scope:\nsub rsp, 0x28\n";

        //debug printing all the global defs
        for (auto i: total_global_consts) {
            of << "mov rcx, DBG_PRINT\nmov rdx, " << i << "\nmov r8, [rel u64_" << i << "]\ncall printf\n";
        }
        of << "add rsp, 0x28\nret\n";
        of.close();
        return {""};
    }
}
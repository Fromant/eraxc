#include <fstream>
#include "asm_translator.h"


namespace blck::backend {


    std::string asm_translator::translate_NASM(const std::vector<syntax::AST::node> &tree,
                                               const std::string &o_filename) {

        //there will be variables (.data section)
        std::vector<data_label> ints_labels{};
        std::stringstream ss{};
        //generate .text

        for (const auto &node: tree) {
            if (node.type == syntax::AST::node::DECLARATION) {
                const auto &decl = node.d.decl;

                data_label temp{};
                temp.label = "u64_" + std::to_string(decl.id);
                if (node.d.decl.assign_to.op == syntax::NONE) {
                    temp.init_val = 0;
                } else if (node.d.decl.assign_to.op == syntax::ASSIGN) {
                    if (decl.assign_to.isInstant) {
                        temp.init_val = decl.assign_to.data;
                    } else {
                        temp.init_val = 0;
                        //init in runtime
                        ss << "mov rax, [rel u64_" + std::to_string(decl.id) + "]\n";//load into reg
                        ss << "mov rax, [rel u64_" + std::to_string(decl.assign_to.data) + "]\n"; //init
                        ss << "mov [rel u64_" + std::to_string(decl.id) + "], rax\n"; //save
                    }
                }
                ints_labels.emplace_back(temp);
            }
        }



        //all to file
        std::ofstream of{o_filename};

        of << "global main\nbits 64\nextern puts\nsection .data\n";
        //copy vars
        for (const auto &s: ints_labels) {
            of << s.label << " dq " << s.init_val << '\n';
        }
        //copy data
        of << "section .text\nmain:\n";
        of << ss.str();
        of<<"sub rsp, 28h\nmov rcx, message\ncall puts\nadd rsp, 28h\nret\nmessage: db 'Hello',0\n";
        of.close();
        return {""};
    }
}
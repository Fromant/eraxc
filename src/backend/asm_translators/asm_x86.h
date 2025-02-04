//
// Created by frama on 01.02.2025.
//

#ifndef BLCK_COMPILER_ASM_X86_H
#define BLCK_COMPILER_ASM_X86_H

#include <ostream>
#include <set>

#include "asm_translator.h"

namespace eraxc {
    template <>
    struct asm_translator<X64> {
        enum x86_reg {
            RAX, RBX, RCX, RDX,

            RBP, RSP,
            RSI, RDI,

            R8, R9, R10, R11,
            R12, R13, R14, R15
        }; //R stands for 64 bits

        static std::string type(u64 type) {
            if (type == syntax::i8 || type == syntax::u8) {
                return "db";
            }
            if (type == syntax::i16 || type == syntax::u16) {
                return "dw";
            }
            if (type == syntax::i32 || type == syntax::u32) {
                return "dd";
            }
            if (type == syntax::i64 || type == syntax::u64) {
                return {"dq"};
            }
            return {"ILLEGAL TYPE"};
        }

        static std::string reg_name(x86_reg reg) {
            switch (reg) {
            case RAX:
                return "RAX";
            case RBX:
                return "RBX";
            case RCX:
                return "RCX";
            case RDX:
                return "RDX";
            case RBP:
                return "RBP";
            case RSP:
                return "RSP";
            case RSI:
                return "RSI";
            case RDI:
                return "RDI";
            case R8:
                return "R8";
            case R9:
                return "R9";
            case R10:
                return "R10";
            case R11:
                return "R11";
            case R12:
                return "R12";
            case R13:
                return "R13";
            case R14:
                return "R14";
            case R15:
                return "R15";
            default:
                return "ILLREG";
            }
        }

        static u64 size(u64 type) {
            switch (type) {
            case syntax::i8:
            case syntax::u8:
                return 1;
            case syntax::i16:
            case syntax::u16:
                return 2;
            case syntax::i32:
            case syntax::u32:
                return 4;
            case syntax::i64:
            case syntax::u64:
                return 8;
            default:
                return -1;
            }
        }

        static constexpr x86_reg pass_ABI[] = {RCX, RDX, R8, R9};

        struct memory_state {
            //DO not use rbp and rsp for now
            std::set<x86_reg> free_regs{RAX, RBX, RCX, RDX, RDI, RSI, R8, R9, R10, R11, R12, R13, R14, R15};

            //For mapping used vars to regs
            std::unordered_map<u64, x86_reg> used_regs{};


            //also represents rsp change from start of stack allocating
            u64 used_stack_space = 0;
            int args_in_registers_count = 0;

            //For mapping used vars to stack
            std::unordered_map<u64, u64> stack_offsets{};

            error::errable<std::string> get_var(u64 var) {
                if (stack_offsets.contains(var)) {
                    u64 offset = used_stack_space - stack_offsets.at(var);
                    if (offset == 0) return {"", "[rsp]"};
                    return {"", "QWORD[rsp+" + std::to_string(offset) + ']'};
                }
                if (used_regs.contains(var)) {
                    return {"", reg_name(used_regs[var])};
                }
                return {"Variable " + std::to_string(var) + " is not allocated", ""};
            }

            error::errable<std::string> allocate_stack_space(int size, u64 var) {
                #ifdef DEBUG //some unuseful check. Delete later
                if (stack_offsets.contains(var) || used_regs.contains(var)) {
                    return {"Variable $" + std::to_string(var) + " is already allocated\n", ""};
                }
                #endif
                stack_offsets[var] = used_stack_space;
                used_stack_space += size;
                return {"", "sub rsp, " + std::to_string(size) + '\n'};
            }

            bool is_allocated(u64 var) const {
                return used_regs.contains(var) || stack_offsets.contains(var);
            }

        };

        memory_state mem{};

        error::errable<void> print_IL_node_asm(const IL::IL_node& node, std::ostream& os) {
            if (node.op == IL::IL_node::RET) {
                auto ret_val = get_operand(node.operand1);
                if (!ret_val) return {ret_val.error};
                os << "mov rax, " << ret_val.value << '\n';
                return {""};
            }
            if (node.op == IL::IL_node::PASS_ARG) {
                //pass arguments
                auto op1 = get_operand(node.operand1);
                if (!op1) return {op1.error};
                os << "mov " << reg_name(pass_ABI[mem.args_in_registers_count++]) << ", " << op1.value << '\n';
            }
            if (node.op == IL::IL_node::JUMP) {
                //handle jump differently
            }
            if (node.op == IL::IL_node::CALL) {
                os << "call $f_" << node.operand1.id << '\n';
                return {""};
            }

            //other instruction needs to allocate assignee (on stack for now)

            auto assignee = mem.allocate_stack_space(size(node.assignee_type), node.assignee);
            if (!assignee) return {"Failed to allocate stack space"};
            os << assignee.value;
            assignee = mem.get_var(node.assignee);
            if (!assignee) return {"Failed to get stack space"};

            //choose instruction better. Check if instant.

            if (node.op == IL::IL_node::ASSIGN) {
                if (node.operand1.is_instant) {
                    os << "mov " << assignee.value << ", " << node.operand1.id << '\n';
                } else {
                    auto op1 = mem.get_var(node.operand1.id);
                    if (!op1) return {op1.error};
                    //if move operand is located on stack, spill him to rax and then do move
                    if (mem.stack_offsets.contains(node.operand1.id)) {
                        os << "mov rax, " << op1.value << '\n';
                        os << "mov " << assignee.value << ", rax\n";
                    } else {
                        //move operand contained in register
                        os << "mov " << assignee.value << ", " << op1.value << '\n';
                    }
                }
                return {""};
            }

            auto op1 = get_operand(node.operand1);
            auto op2 = get_operand(node.operand2);
            if (!op1) return {op1.error};
            if (!op2) return {op2.error};

            //mov op1 to rax
            os << "mov rax, " << op1.value << '\n';

            if (node.op == IL::IL_node::ADD) {
                os << "add rax, " << op2.value << '\n';
            } else if (node.op == IL::IL_node::SUB) {
                os << "sub rax, " << op2.value << '\n';
            } else if (node.op == IL::IL_node::MUL) {
                os << "imul rax, " << op2.value << '\n';
            } else if (node.op == IL::IL_node::DIV) {
                //For now use idiv (that's slow)
                os << "xor rdx, rdx ;Divide operation\n"; //Dividend top half
                os << "mov rbx, " << op2.value << '\n'; //Divisor
                os << "idiv rbx\n"; // Do divide. Modulo is now in rdx.
            } else if (node.op == IL::IL_node::MODULO) {
                //For now use idiv (that's slow)
                os << "xor rdx, rdx ;Modulo operation\n"; //Dividend top half
                os << "mov rbx, " << op2.value << '\n'; //Divisor
                os << "idiv rbx\n"; // Do divide. Modulo is now in rdx
                os << "mov " << assignee.value << ", rdx\n";
                return {""};
            } else if (node.op == IL::IL_node::NOT) {
                os << "not rax, " << op2.value << '\n';
            } else if (node.op == IL::IL_node::NEG) {
                os << "neg rax, " << op2.value << '\n';
            } else if (node.op == IL::IL_node::AND) {
                os << "and rax, " << op2.value << '\n';
            } else if (node.op == IL::IL_node::OR) {
                os << "or rax, " << op2.value << '\n';
            } else if (node.op == IL::IL_node::XOR) {
                os << "xor rax, " << op2.value << '\n';
            } else if (node.op == IL::IL_node::LSHIFT) {
                os << "shl rax, " << op2.value << '\n';
            } else if (node.op == IL::IL_node::RSHIFT) {
                os << "shr rax, " << op2.value << '\n';
            }

            //save result to assignee
            os << "mov " << assignee.value << ", rax\n";

            return {""};
        }

        error::errable<std::string> get_operand(const IL::IL_operand& op) {
            if (op.is_instant) { return {"", std::to_string(op.id)}; }
            return mem.get_var(op.id);
        }

        error::errable<void> translate(const IL::IL_handler& IL_handler, const std::string& o_filename) {
            std::ofstream file{o_filename};

            if (!file) return {"Failed to open output file " + o_filename};

            file << "global main\nbits 64\nextern printf\nsection .data\n";

            //print globals
            for (const auto& global_var : IL_handler.global_scope.identifiers) {
                if (global_var.second.isfunc) continue;
                file << "var$" << global_var.second.id << ' ' << type(global_var.second.type) << " 0\n";
            }

            file << "DBG_PRINT: db \"{%d}: %d\", 0x0A, 0x00\n"
                "section .text\n"
                "main:\n"
                "sub rsp, 0x28\n"
                "call init_global_scope\n";

            //add main function there
            auto main_it = IL_handler.global_scope.identifiers.find("main");
            if (main_it == IL_handler.global_scope.identifiers.end() || main_it->second.isfunc == false ||
                !IL_handler.global_funcs.at(main_it->second.id).args.empty()) {
                return {"Failed to find main function"};
            }
            const std::vector<IL::IL_node>& il = IL_handler.global_funcs.at(main_it->second.id).body;
            for (const auto& il_node : il) {
                auto r = print_IL_node_asm(il_node, file);
                if (!r) return r;
            }

            //now return rsp to where it's been before allocations
            file << "add rsp, 0x" << std::hex << mem.used_stack_space + 0x28 << std::dec << '\n';

            file << "ret;\n"
                "init_global_scope:\n"
                "sub rsp, 0x08\n"; //why?

            //add init global scope there
            file << "; insert global scope init here\n";


            file << "add rsp, 0x08\n"
                "ret\n";

            return {""};
        }
    };
}

#endif //BLCK_COMPILER_ASM_X86_H

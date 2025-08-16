#ifndef BLCK_COMPILER_ASM_X86_H
#define BLCK_COMPILER_ASM_X86_H

#include <ostream>
#include <ranges>
#include <set>

#include "asm_translator.h"
#include "asm_x86_mem.h"

namespace eraxc {
    template<>
    struct asm_translator<X64> {

        static std::string type(u64 type) {
            if (type == syntax::i8 || type == syntax::u8) { return "db"; }
            if (type == syntax::i16 || type == syntax::u16) { return "dw"; }
            if (type == syntax::i32 || type == syntax::u32) { return "dd"; }
            if (type == syntax::i64 || type == syntax::u64) { return "dq"; }
            return {"ILLTYPE"};
        }

        static u64 size(u64 type) {
            switch (type) {
                case syntax::i8:
                case syntax::u8: return 1;
                case syntax::i16:
                case syntax::u16: return 2;
                case syntax::i32:
                case syntax::u32: return 4;
                case syntax::i64:
                case syntax::u64: return 8;
                default: return -1;
            }
        }

        memory_state mem {};
        std::set<size_t> printed_nodes;

        error::errable<void> print_JIR_node_asm(const JIR::Node& node, std::ostream& os) {
            if (node.op == JIR::Operation::NONE) { return {""}; }

            if (node.op == JIR::Operation::LABEL) {
                os << ".l" << node.operand1.value << ":\n";
                return {""};
            }

            if (node.op == JIR::Operation::INC) {
                auto op1 = get_operand(node.operand1);
                if (!op1) return {op1.error};

                os << "inc " << op1.value << '\n';

                return {""};
            }
            if (node.op == JIR::Operation::DEC) {
                auto op1 = get_operand(node.operand1);
                if (!op1) return {op1.error};

                os << "dec " << op1.value << '\n';

                return {""};
            }
            if (node.op == JIR::Operation::RET) {
                auto ret_val = get_operand(node.operand1);
                if (!ret_val) return {ret_val.error};
                std::string reg = reg_name(x86_reg::RAX, size(node.operand1.type));
                os << "mov " << reg << ", " << ret_val.value << '\n';
                return {""};
            }
            if (node.op == JIR::Operation::PASS) {
                //pass arguments
                auto op1 = get_operand(node.operand1);
                if (!op1) return {op1.error};
                os << "mov " << reg_name(pass_ABI[mem.args_in_registers_count++], size(node.operand1.type)) << ", "
                   << op1.value << '\n';
                return {""};
            }
            if (node.op == JIR::Operation::JUMP) {
                //handle jump differently
                os << "jmp .l" << node.operand1.value << '\n';
                return {""};
            }
            if (node.op == JIR::Operation::JE) {
                //handle jump differently
                os << "je .l" << node.operand1.value << '\n';
                return {""};
            }
            if (node.op == JIR::Operation::JNE) {
                //handle jump differently
                os << "jne .l" << node.operand1.value << '\n';
                return {""};
            }
            if (node.op == JIR::Operation::JG) {
                //handle jump differently
                os << "jg .l" << node.operand1.value << '\n';
                return {""};
            }
            if (node.op == JIR::Operation::JGE) {
                //handle jump differently
                os << "jge .l" << node.operand1.value << '\n';
                return {""};
            }
            if (node.op == JIR::Operation::JL) {
                //handle jump differently
                os << "jl .l" << node.operand1.value << '\n';
                return {""};
            }
            if (node.op == JIR::Operation::JLE) {
                //handle jump differently
                os << "jle .l" << node.operand1.value << '\n';
                return {""};
            }
            if (node.op == JIR::Operation::CALL) {
                auto op2 = mem.get_var(node.operand2.value, size(node.operand2.type));
                if (!op2) return {op2.error};
                os << "call $f_" << node.operand1.value << '\n';
                std::string reg = reg_name(x86_reg::RAX, size(node.operand1.type));
                os << "mov " << op2.value << ", " << reg << '\n';
                mem.args_in_registers_count = 0;
                return {""};
            }
            if (node.op == JIR::Operation::ALLOC) {
                auto assignee = mem.allocate_stack_space(size(node.operand1.type), node.operand1.value);
                if (!assignee) return {"Failed to allocate stack space"};
                os << assignee.value;
                return {""};
            }
            if (node.op == JIR::Operation::DEALLOC) {
                auto assignee = mem.try_dealloc(size(node.operand1.type), node.operand1.value);
                if (!assignee) return {"Failed to deallocate stack space: " + assignee.error};
                os << assignee.value;
                return {""};
            }

            //TODO choose instruction better. Check if instant.

            auto op1 = get_operand(node.operand1);
            auto op2 = get_operand(node.operand2);
            if (!op1) return {op1.error};
            if (!op2) return {op2.error};

            if (node.op == JIR::Operation::MOVE) {
                if (node.operand2.is_instant) {
                    os << "mov " << op1.value << ", " << node.operand2.value << '\n';
                } else {
                    //if move operand is located on stack, spill him to rax and then do move
                    if (mem.stack_offsets.contains(node.operand2.value)) {
                        std::string reg = reg_name(x86_reg::RAX, size(node.operand2.type));
                        os << "mov " << reg << ", " << op2.value << '\n';
                        os << "mov " << op1.value << ", " << reg << '\n';
                    } else {
                        //move operand contained in register
                        os << "mov " << op1.value << ", " << op2.value << '\n';
                    }
                }
                return {""};
            }

            //mov op1 to rax
            //TODO if already in register there's no need in this
            std::string reg = reg_name(x86_reg::RAX, size(node.operand1.type));
            os << "mov " << reg << ", " << op1.value << '\n';

            if (node.op == JIR::Operation::ADD) {
                os << "add " << reg << ", " << op2.value << '\n';
            } else if (node.op == JIR::Operation::SUB) {
                os << "sub " << reg << ", " << op2.value << '\n';
            } else if (node.op == JIR::Operation::MUL) {
                os << "imul " << reg << ", " << op2.value << '\n';
            } else if (node.op == JIR::Operation::DIV) {
                //For now use idiv (that's slow)
                os << "xor rdx, rdx ;Divide operation\n";  //Dividend top half
                os << "mov rbx, " << op2.value << '\n';  //Divisor
                os << "idiv rbx\n";  // Do divide. Modulo is now in rdx.
            } else if (node.op == JIR::Operation::MOD) {
                //For now use idiv (that's slow)
                os << "xor rdx, rdx ;Modulo operation\n";  //Dividend top half
                os << "mov rbx, " << op2.value << '\n';  //Divisor
                os << "idiv rbx\n";  // Do divide. Modulo is now in rdx
                os << "mov " << op1.value << ", rdx\n";
                return {""};
            } else if (node.op == JIR::Operation::NOT) {
                os << "not " << reg << ", " << op2.value << '\n';
            } else if (node.op == JIR::Operation::NEG) {
                os << "neg " << reg << ", " << op2.value << '\n';
            } else if (node.op == JIR::Operation::AND) {
                os << "and " << reg << ", " << op2.value << '\n';
            } else if (node.op == JIR::Operation::OR) {
                os << "or " << reg << ", " << op2.value << '\n';
            } else if (node.op == JIR::Operation::XOR) {
                os << "xor " << reg << ", " << op2.value << '\n';
            } else if (node.op == JIR::Operation::LSHIFT) {
                os << "shl " << reg << ", " << op2.value << '\n';
            } else if (node.op == JIR::Operation::RSHIFT) {
                os << "shr " << reg << ", " << op2.value << '\n';
            } else if (node.op == JIR::Operation::CMP) {
                os << "cmp " << reg << ", " << op2.value << '\n';
                return {""}; //return without saving from reg to stack
            }
            //save result to assignee
            os << "mov " << op1.value << ", " << reg << "\n";

            return {""};
        }

        error::errable<std::string> get_operand(const JIR::Operand& op) {
            if (op.is_instant) { return {"", std::to_string(op.value)}; }
            return mem.get_var(op.value, size(op.type));
        }


        error::errable<void> print_cfg_node(const JIR::CFG& cfg, size_t node_id, std::ostream& os) {

            // if (printed_nodes.contains(node_id)) return {""};

            const JIR::CFG_Node& node = cfg.get_cfg_node(node_id);

            //allocs
            for (const auto& alloc : node.allocations) {
                if (mem.used_regs.contains(alloc.value)) continue;
                auto allocation = mem.allocate_stack_space(size(alloc.type), alloc.value);
                if (!allocation) return {"Failed to allocate stack space: " + allocation.error};
                os << allocation.value;
            }
            //body
            for (const auto& JIR_node : node.body) {
                auto print = print_JIR_node_asm(JIR_node, os);
                if (!print) return print;
            }

            //print subnodes
            const auto& edges = cfg.get_edges().equal_range(node_id);
            for (auto i = edges.first; i != edges.second; ++i) {
                os << ".l" << i->second << ":\n";
                auto r1 = print_cfg_node(cfg, i->second, os);
                if (!r1) return r1;
                // os << "add rsp, 8\nret\n";
            }

            //deallocs
            for (const auto& allocation : std::ranges::reverse_view(node.allocations)) {
                auto ddd = mem.try_dealloc(size(allocation.type), allocation.value);
                if (!ddd) return {"Failed to deallocate variable: " + ddd.error};
                os << ddd.value;
            }

            printed_nodes.insert(node_id);

            return {""};
        }

        error::errable<void> translate(const JIR::CFG& cfg, const std::string& o_filename) {
            std::ofstream file {o_filename};

            if (!file) return {"Failed to open output file " + o_filename};

            file << "global main\nbits 64\nextern printf\nsection .data\n";

            const Scope& global_scope = cfg.get_scope(0);

            //print globals
            for (const auto& global_var : global_scope.identifiers) {
                if (global_var.second.is_func()) continue;
                file << "var$" << global_var.second.id << ' ' << type(global_var.second.get_type()) << " 0\n";
                mem.globals.insert(global_var.second.id);
            }

            file << "DBG_PRINT: db \"{%d}: %d\", 0x0A, 0x00\n"
                    "section .text\n"
                    "main:\n"
                    "sub rsp, 0x28\n"
                    "call $f_0\n"
                 <<
                //call to main function
                "call $f_" << cfg.get_funcs().at(global_scope.identifiers.find("main")->second.id).node_id << '\n';

            //now return rsp to where it's been before allocations
            file << "add rsp, 0x" << std::hex << mem.used_stack_space + 0x28 << std::dec << '\n';
            mem.reset();

            file << "ret;\n";

            //print global init
            {
                file << "$f_0:\nsub rsp, 8\n";
                auto r = print_cfg_node(cfg, 0, file);
                if (!r) return r;
                file << "add rsp, 8\nret\n";
            }

            //now print all functions
            for (const auto& func : cfg.get_funcs()) {
                //TODO handle args pass correctly (only 4 params would fit in ABI)
                for (const auto& param : func.second.params) {
                    mem.used_regs.emplace(param.value, pass_ABI[mem.args_in_registers_count++]);
                }
                mem.args_in_registers_count = 0;

                file << "$f_" << func.second.node_id << ":\nsub rsp, 8\n";
                auto r = print_cfg_node(cfg, func.second.node_id, file);
                file << "add rsp, 8\nret\n";

                if (!r) return r;

                mem.reset();
            }
            return {""};
        }
    };
}

#endif  //BLCK_COMPILER_ASM_X86_H

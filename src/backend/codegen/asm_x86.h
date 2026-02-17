#pragma once

#include <ostream>
#include <queue>
#include <set>

#include "asm_x86_mem.h"
#include "backend/JIR/CFG/CFG.h"

namespace eraxc::x86 {
    struct asm_translator {

        memory_state mem {};
        std::set<size_t> printed_nodes;

        error::errable<void> print_JIR_node_asm(const JIR::JIRop& node, std::ostream& os) {
            if (node.op == JIR::Operation::NONE) {
                return {""};
            }

            if (node.op == JIR::Operation::LABEL) {
                os << ".l" << node.operand1.value << ":\n";
                return {""};
            }

            if (node.op == JIR::Operation::INC) {
                auto op1 = get_operand(node.operand1);
                if (!op1)
                    return {op1.error};

                os << "inc " << op1.value << '\n';

                return {""};
            }
            if (node.op == JIR::Operation::DEC) {
                auto op1 = get_operand(node.operand1);
                if (!op1)
                    return {op1.error};

                os << "dec " << op1.value << '\n';

                return {""};
            }
            if (node.op == JIR::Operation::RET) {
                os << "add rsp, " << mem.used_stack_space << '\n';
                return {""};
            }
            if (node.op == JIR::Operation::PASS) {
                //pass arguments
                auto op1 = get_operand(node.operand1);
                if (!op1)
                    return {op1.error};
                os << "mov " << reg_name(pass_ABI[mem.args_in_registers_count++], size(node.operand1.type)) << ", "
                   << op1.value << '\n';
                return {""};
            }
            if (node.op == JIR::Operation::PASS_RET) {
                auto op1 = get_operand(node.operand1);
                if (!op1)
                    return {op1.error};
                os << "mov " << reg_name(x86_reg::RAX, size(node.operand1.type)) << ", " << op1.value << '\n';
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
                if (!op2)
                    return {op2.error};
                const auto diff = (mem.used_stack_space + 8) % 16;
                if (diff != 0)
                    os << "sub rsp, " << 16 - diff << '\n';
                os << "call $f_" << node.operand1.value << '\n';
                if (diff != 0)
                    os << "add rsp, " << 16 - diff << '\n';
                std::string reg = reg_name(x86_reg::RAX, size(node.operand1.type));
                os << "mov " << op2.value << ", " << reg << '\n';
                mem.args_in_registers_count = 0;
                return {""};
            }
            if (node.op == JIR::Operation::ALLOC) {
                auto assignee = mem.allocate_stack_space(size(node.operand1.type), node.operand1.value);
                if (!assignee)
                    return {"Failed to allocate stack space: " + assignee.error};
                os << assignee.value;
                return {""};
            }
            if (node.op == JIR::Operation::DEALLOC) {
                auto assignee = mem.try_dealloc(size(node.operand1.type), node.operand1.value);
                if (!assignee)
                    return {"Failed to deallocate stack space: " + assignee.error};
                os << assignee.value;
                return {""};
            }

            //TODO choose instruction better. Check if instant.

            // if (node.operand1.is_instant)
            // if (node.operand2.is_instant)

            auto op1 = get_operand(node.operand1);
            auto op2 = get_operand(node.operand2);
            if (!op1)
                return {op1.error};
            if (!op2)
                return {op2.error};

            if (node.op == JIR::Operation::MOVE) {
                if (node.operand2.is_instant) {
                    os << "mov " << op1.value << ", " << node.operand2.value << '\n';
                } else {
                    //if move operand is located on stack, spill him to rax and then do move
                    if (mem.stack_offsets.contains(node.operand2.value) || mem.globals.contains(node.operand2.value)) {
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
                return {""};  //return without saving from reg to stack
            }
            //save result to assignee
            os << "mov " << op1.value << ", " << reg << "\n";

            return {""};
        }

        error::errable<std::string> get_operand(const JIR::Operand& op) {
            if (op.is_instant) {
                return {"", std::to_string(op.value)};
            }
            return mem.get_var(op.value, size(op.type));
        }


        error::errable<void> print_cfg_node(const JIR::CFG& cfg, size_t node_id, std::ostream& os) {
            // insert and check
            auto [it, inserted] = printed_nodes.emplace(node_id);
            if (!inserted) {
                return {""};
            }

            // BFS queue
            std::queue<size_t> q;
            q.push(node_id);

            while (!q.empty()) {
                size_t current_id = q.front();
                q.pop();

                const JIR::CFG_Node& node = cfg.get_cfg_node(current_id);

                // Print body
                os << ".l" << current_id << ":\n";
                for (const auto& JIR_node : node.body) {
                    auto print = print_JIR_node_asm(JIR_node, os);
                    if (!print) {
                        return print;
                    }
                }

                const auto& [start, end] = cfg.get_edges().equal_range(current_id);

                // Iterate through edges
                for (auto i = start; i != end; ++i) {
                    if (i->second.type == JIR::SQUASH) {
                        // dealloc stack
                        const auto r = mem.try_dealloc_stack_space(node.scope.getAllocatedSize());
                        if (!r) {
                            return r.error;
                        }
                    }
                    auto jump_print = print_JIR_node_asm(
                        {i->second.jump_op, JIR::Operand {0, i->second.to_id, false, false}, JIR::Operand {}}, os);
                    if (!jump_print) {
                        return jump_print;
                    }

                    // Add to queue if not visited
                    auto [child_it, child_inserted] = printed_nodes.emplace(i->second.to_id);
                    if (child_inserted) {
                        q.push(i->second.to_id);
                    }
                }
            }

            return {""};
        }

        error::errable<void> translate(const JIR::CFG& cfg, const std::string& o_filename) {
            std::ofstream file {o_filename};

            if (!file)
                return {"Failed to open output file " + o_filename};

            file << "global main\nbits 64\nextern printf\nsection .data\n";

            //print globals
            for (const auto& it : cfg.getScopeManager().top().getIdentifiers()) {
                if (it.second.isFunc()) {
                    continue;
                }
                file << "var$" << it.second.getId() << ' ' << type(it.second.getType()) << " 0\n";
                mem.globals.insert(it.second.getId());
            }

            file << "DBG_PRINT: db \"{%d}: %d\", 0x0A, 0x00\n"
                    "section .text\n"
                    "main:\n"
                    "sub rsp, 0x28\n"
                    "call $f_0\n";
            //TODO move global initialization to separate cfg node that is always presented

            if (const auto main_id = cfg.getScopeManager().findIdRecursive("main"); main_id && main_id.value() != 0) {
                file << "call $f_" << main_id.value() << '\n';
            }

            //now return rsp to where it's been before allocations
            file << "add rsp, 0x" << std::hex << mem.used_stack_space + 0x28 << std::dec << '\n';
            mem.reset();

            file << "ret;\n";

            //print global init
            if (!cfg.get_nodes()[0].body.empty()) {
                file << "$f_0:\nsub rsp, 8\n";
                auto r = print_cfg_node(cfg, 0, file);
                if (!r)
                    return r;
                file << "add rsp, " << 8 + mem.used_stack_space << "\nret\n";
                mem.used_stack_space = 0;
                // file << "add rsp, 8\nret\n";
                mem.reset();
            }

            //now print all functions
            for (const auto& func : cfg.get_funcs()) {
                //TODO handle args pass correctly (only 4 params would fit in ABI)
                for (const auto& param : func.second.params) {
                    mem.used_regs.emplace(param.value, pass_ABI[mem.args_in_registers_count++]);
                }
                mem.args_in_registers_count = 0;

                file << "$f_" << func.first << ":\nsub rsp, 8\n";
                auto r = print_cfg_node(cfg, func.second.node_id, file);
                file << "add rsp, 8\nret\n";

                if (!r)
                    return r;

                mem.reset();
            }
            return {""};
        }
    };
}

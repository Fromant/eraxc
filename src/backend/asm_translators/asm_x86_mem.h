//
// Created by frama on 25.02.2025.
//

#ifndef ASM_X86_MEM_H
#define ASM_X86_MEM_H

namespace eraxc {
    enum class x86_reg {
        RAX,
        RBX,
        RCX,
        RDX,

        RBP,
        RSP,
        RSI,
        RDI,

        R8,
        R9,
        R10,
        R11,
        R12,
        R13,
        R14,
        R15
    };  //R stands for 64 bits
    static std::string reg_name(x86_reg reg, u64 type_size) {
        std::string r;
        if (type_size == 8) {
            r = "r";
        } else if (type_size == 4) {
            r = "d";
        } else return {"ILLREG SIZE"};
        switch (reg) {
            case x86_reg::RAX: r += "ax"; break;
            case x86_reg::RBX: r += "bx"; break;
            case x86_reg::RCX: r += "cx"; break;
            case x86_reg::RDX: r += "dx"; break;
            case x86_reg::RBP: r += "bp"; break;
            case x86_reg::RSP: r += "sp"; break;
            case x86_reg::RSI: r += "si"; break;
            case x86_reg::RDI: r += "di"; break;
            case x86_reg::R8: r += "8"; break;
            case x86_reg::R9: r += "9"; break;
            case x86_reg::R10: r += "10"; break;
            case x86_reg::R11: r += "11"; break;
            case x86_reg::R12: r += "12"; break;
            case x86_reg::R13: r += "13"; break;
            case x86_reg::R14: r += "14"; break;
            case x86_reg::R15: r += "15"; break;
            default: return "ILLREG";
        }
        return r;
    }


    static constexpr x86_reg pass_ABI[] = {x86_reg::RCX, x86_reg::RDX, x86_reg::R8, x86_reg::R9};

    struct memory_state {
        //DO not use rbp and rsp for now
        std::set<x86_reg> free_regs {x86_reg::RAX, x86_reg::RBX, x86_reg::RCX, x86_reg::RDX, x86_reg::RDI,
                                     x86_reg::RSI, x86_reg::R8,  x86_reg::R9,  x86_reg::R10, x86_reg::R11,
                                     x86_reg::R12, x86_reg::R13, x86_reg::R14, x86_reg::R15};

        //For mapping used vars to regs
        std::unordered_map<u64, x86_reg> used_regs {};


        //also represents rsp change from start of stack allocating
        u64 used_stack_space = 0;
        int args_in_registers_count = 0;

        //For mapping used vars to stack
        std::unordered_map<u64, u64> stack_offsets {};

        std::set<u64> globals {};

        error::errable<std::string> get_var(u64 var, u64 type_size) {
            if (globals.contains(var)) { return {"", "QWORD[rel var$" + std::to_string(var) + "]"}; }
            if (stack_offsets.contains(var)) {
                u64 offset = used_stack_space - stack_offsets.at(var);
                if (offset == 0) {
                    if (type_size == 8) return {"", "QWORD[rsp]"};
                    if (type_size == 4) return {"", "DWORD[rsp]"};
                    return {"Unsupported size", {}};
                }
                if (type_size == 8) return {"", "QWORD[rsp+" + std::to_string(offset) + ']'};
                if (type_size == 4) return {"", "DWORD[rsp+" + std::to_string(offset) + ']'};
                return {"Unsupported size", {}};
            }
            if (used_regs.contains(var)) { return {"", reg_name(used_regs[var], type_size)}; }
            return {"Variable " + std::to_string(var) + " is not allocated", ""};
        }

        error::errable<std::string> allocate_stack_space(int size, u64 var) {
#ifdef DEBUG  //some unuseful check. Delete later
            if (stack_offsets.contains(var) || used_regs.contains(var)) {
                return {"Variable $" + std::to_string(var) + " is already allocated\n", ""};
            }
#endif
            stack_offsets[var] = used_stack_space;
            used_stack_space += size;
            return {"", "sub rsp, " + std::to_string(size) + '\n'};
        }

        bool is_allocated(u64 var) const { return used_regs.contains(var) || stack_offsets.contains(var); }

        void reset() {
            used_stack_space = 0;
            used_regs.clear();
            args_in_registers_count = 0;
            stack_offsets.clear();
        }
    };
}
#endif  //ASM_X86_MEM_H

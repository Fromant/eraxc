#include "asm_x64.hpp"

#include "backend/JIR/CFG/Allocated/Iterator/CFGIterator.hpp"


error::errable<std::string> get_asm_size_from_type_size(size_t type_size) {
    if (type_size == 8) {
        return {"", "QWORD"};
    }
    if (type_size == 4) {
        return {"", "DWORD"};
    }
    return {"Codegen: unsupported type size " + std::to_string(type_size), ""};
}

error::errable<std::string> asm_translator::get_operand(const JIR::Allocated::OperandAllocated& op) {
    const size_t type_size = size(op.type);

    if (op.place == JIR::Allocated::OperandAllocated::INSTANT) {
        return {"", std::to_string(op.value)};
    }

    if (op.place == JIR::Allocated::OperandAllocated::REGISTER) {
        return {"", reg_name((x86_reg)op.value, type_size)};
    }

    auto size_errable = get_asm_size_from_type_size(type_size);
    if (!size_errable) {
        return size_errable;
    }
    const std::string size = size_errable.value;

    if (op.place == JIR::Allocated::OperandAllocated::STACK) {
        u64 offset = op.value;
        if (offset == 0) {
            return {"", size + "[rsp]"};
        }
        return {"", size + "[rsp+" + std::to_string(offset) + ']'};
    }

    if (op.place == JIR::Allocated::OperandAllocated::GLOBAL) {
        return {"", size + "[rel global$" + std::to_string(op.value) + "]"};
    }

    return {"Codegen: unknown place of operand: " + std::to_string(op.place), {}};
}
error::errable<void> asm_translator::print_JIR_node_asm(const JIR::Allocated::JIRAOp& node, std::ostream& os,
                                                        size_t stackSize) {
    if (node.op == JIR::Operation::NONE) {
        return {""};
    }

    if (node.op == JIR::Operation::LABEL) {
        os << ".l" << node.operand1.value << ":\n";
        return {""};
    }

    if (node.op == JIR::Operation::INC) {
        auto op1 = get_operand(node.operand1);
        if (!op1) {
            return {op1.error};
        }
        os << "inc " << op1.value << '\n';
        return {""};
    }
    if (node.op == JIR::Operation::DEC) {
        auto op1 = get_operand(node.operand1);
        if (!op1) {
            return {op1.error};
        }

        os << "dec " << op1.value << '\n';

        return {""};
    }
    if (node.op == JIR::Operation::RET) {
        os << "add rsp, " << stackSize << '\n';
        os << "ret" << std::endl;
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
        const auto diff = stackSize % 16;
        if (diff != 0) {
            os << "sub rsp, " << 16 - diff << '\n';
        }
        os << "call $f_" << node.operand1.value << '\n';
        if (diff != 0) {
            os << "add rsp, " << 16 - diff << '\n';
        }
        return {""};
    }
    if (node.op == JIR::Operation::ALLOC) {
        return {"ERROR: How alloc cmd is here?"};
    }
    if (node.op == JIR::Operation::DEALLOC) {
        return {"ERROR: How dealloc cmd is here?"};
    }
    if (node.op == JIR::Operation::STACKALLOC) {
        // return {"ERROR: How STACKALLOC cmd is here?"};
        return "";
    }
    if (node.op == JIR::Operation::STACKDEALLOC) {
        // return {"ERROR: How STACKDEALLOC cmd is here?"};
        return "";
    }

    //TODO choose instruction better. Check if instant.

    // if (node.operand1.is_instant)
    // if (node.operand2.is_instant)

    auto op1 = get_operand(node.operand1);
    if (!op1) {
        return {op1.error};
    }
    auto op2 = get_operand(node.operand2);
    if (!op2) {
        return {op2.error};
    }

    if (node.op == JIR::Operation::MOVE) {
        if (node.operand2.place == JIR::Allocated::OperandAllocated::INSTANT) {
            os << "mov " << op1.value << ", " << node.operand2.value << '\n';
            return {""};
        }

        // in x64 all move ops with moffs (global reg, like DWORD[rel global$0]) can only be through RAX reg
        if (node.operand1.place == JIR::Allocated::OperandAllocated::GLOBAL &&
            node.operand2.place == JIR::Allocated::OperandAllocated::GLOBAL) {
            //global to global case
            std::string regA = reg_name(x86_reg::RAX, size(node.operand1.type));
            // os << "push rax\n";
            os << "mov " << regA << ", " << op2.value << '\n';
            os << "mov " << op1.value << ", " << regA << '\n';
            // os << "pop rax\n";
            return {""};
        }
        if (node.operand1.place == JIR::Allocated::OperandAllocated::GLOBAL) {
            if (node.operand2.place == JIR::Allocated::OperandAllocated::REGISTER &&
                node.operand2.value == (u64)x86_reg::RAX) {
                // already in rax
                os << "mov " << op1.value << ", " << op2.value << '\n';
                return {""};
            }
            std::string regA = reg_name(x86_reg::RAX, size(node.operand1.type));
            // os << "push rax\n";
            os << "mov " << regA << ", " << op2.value << '\n';
            os << "mov " << op1.value << ", " << regA << "\n";
            // os << "pop rax\n";
            return {""};
        }
        if (node.operand2.place == JIR::Allocated::OperandAllocated::GLOBAL) {
            if (node.operand1.place == JIR::Allocated::OperandAllocated::REGISTER &&
                node.operand1.value == (u64)x86_reg::RAX) {
                // already in rax
                os << "mov " << op1.value << ", " << op2.value << '\n';
                return {""};
            }
            std::string regA = reg_name(x86_reg::RAX, size(node.operand1.type));
            // os << "push rax\n";
            os << "mov " << regA << ", " << op2.value << '\n';
            os << "mov " << op1.value << ", " << regA << "\n";
            // os << "pop rax\n";
            return {""};
        }

        // move between two memory is not allowed
        if (node.operand1.place == JIR::Allocated::OperandAllocated::STACK &&
            node.operand2.place == JIR::Allocated::OperandAllocated::STACK) {
            std::string regA = reg_name(x86_reg::RAX, size(node.operand1.type));
            // os << "push rax\n";
            os << "mov " << regA << ", " << op2.value << '\n';
            os << "mov " << op1.value << ", " << regA << '\n';
            // os << "pop rax\n";
            return {""};
        }

        // move between (reg, mem) or (mem, reg)
        os << "mov " << op1.value << ", " << op2.value << '\n';
        return {""};
    }

    std::string reg = reg_name(x86_reg::RAX, size(node.operand1.type));
    //mov op1 to rax
    // if (!node.operand1.is_stack_allocated /*&& ! is global*/) {
    os << "mov " << reg << ", " << op1.value << '\n';
    // }

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
error::errable<void> asm_translator::print_cfg_node(const JIR::Allocated::CFGAllocated& cfg, size_t node_id,
                                                    std::ostream& os, size_t stackSize) {
    auto iter = JIR::CFGIterator {cfg, node_id};
    while (iter) {
        const JIR::Allocated::CFGA_Node& node = *iter;

        os << ".l" << iter.nodeId() << ":\n";
        for (const auto& JIR_node : node.body) {
            auto print = print_JIR_node_asm(JIR_node, os, stackSize);
            if (!print) {
                return print;
            }
        }
        for (const auto& edge : iter.getEdges()) {
            auto jump_print = print_JIR_node_asm(
                {edge.jump_op,
                 JIR::Allocated::OperandAllocated {0, edge.to_id, JIR::Allocated::OperandAllocated::INSTANT},
                 JIR::Allocated::OperandAllocated {}},
                os, stackSize);
            if (!jump_print) {
                return jump_print;
            }
        }
        ++iter;
    }
    return "";
}
error::errable<void> asm_translator::translate(const JIR::Allocated::CFGAllocated& cfg, const std::string& o_filename) {
    std::ofstream file {o_filename};

    if (!file) {
        return {"Failed to open output file " + o_filename};
    }

    file << "global main\nbits 64\nextern printf\nsection .data\n";

    //print globals
    for (const auto& decl : cfg.getGlobals()) {
        if (decl.isFunc()) {
            continue;
        }
        file << "global$" << decl.getId() << ' ' << type(decl.getType()) << " 0\n";
    }

    const auto& main_id = cfg.getScopeManager().findIdRecursive("main");
    if (!main_id) {
        return "Codegen: Can't find entrypoint `main()`";
    }
    file << "DBG_PRINT: db \"{%d}: %d\", 0x0A, 0x00\n"
            "section .text\n"
            "main:\n"
            "sub rsp, 0x28\n";
    if (main_id.value() != 0 && !cfg.getCfgNode(0).body.empty()) {
        // init globals
        file << "call $f_0\n";
    }
    file << "call $f_" << main_id.value() << '\n';
    file << "add rsp, 0x28\n";
    file << "ret;\n";

    const auto& print_globals = [&file, &cfg]() {
        file << "$f_0:\n";
        const auto allocatedStack = cfg.getScopeManager().top().getAllocatedSize();
        file << "sub rsp, " << allocatedStack << '\n';
        auto r = print_cfg_node(cfg, 0, file, allocatedStack);
        file << "add rsp, " << allocatedStack << '\n';
        file << "ret\n";
        return r;
    };

    if (main_id.value() != 0 && !cfg.getCfgNode(0).body.empty()) {
        // print global allocation
        auto global_err = print_globals();
        if (!global_err) {
            return global_err;
        }
    }

    // print all functions
    for (const auto& [id, func] : cfg.getFuncs()) {
        size_t allocated_stack = 8 + func.max_stack_size;
        file << "$f_" << id << ":\nsub rsp, " << allocated_stack << "\n";
        auto r = print_cfg_node(cfg, func.node_id, file, allocated_stack);
        file << "add rsp, " << allocated_stack << "\nret\n";
        if (!r) {
            return r;
        }
    }
    return {""};
}
#include "FrontendResult.hpp"

#include <iostream>
#include <ranges>

#include "common/CFG/CFGIterator.hpp"

using namespace eraxc::JIR;

void FrontendResult::print() const {
    for (const auto& func : functions) {
        std::cout << "Function $" << func.decl.id << '(';
        if (!func.params.empty()) {
            for (const auto& param : func.params | std::views::take(func.params.size() - 1)) {
                std::cout << to_string(param.type) << " $" << param.id << ", ";
            }
            const auto& last = func.params.back();
            std::cout << to_string(last.type) << " $" << last.id;
        }
        std::cout << "):" << std::endl;

        if (func.cfg.nodes.empty()) {
            continue;
        }

        CFG::CFGIterator iter {func.cfg, 0};

        while (iter) {
            std::cout << '.' << iter.nodeId() << ':' << std::endl;
            for (const auto& node : *iter) {
                std::cout << '\t';
                node.print();
            }
            ++iter;
        }
    }
}
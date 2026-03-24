#include "FrontendResult.hpp"

#include <fstream>
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
            for (const auto& node : (*iter).nodes) {
                std::cout << '\t';
                node.print();
            }
            ++iter;
        }
    }
}
void FrontendResult::print_to_file(const std::string& str) const {
    std::ofstream file(str);

    const auto edge_to_int = [](const CFG::CFGEdge::CFGEdgeType t) {
        if (t == CFG::CFGEdge::SQUASH) {
            return 0;
        }
        return 1;
    };

    for (const auto& func : functions) {
        for (const auto& edge : func.cfg.edges) {
            const auto from = edge.first;
            for (const auto& to : edge.second) {
                file << from << ", " << to.to_id << ", " << edge_to_int(to.type) << std::endl;
            }
        }
    }
}
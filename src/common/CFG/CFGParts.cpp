#include "CFGParts.hpp"

using namespace eraxc::CFG;


void CFGNode::operator+=(const CFGNode& node) {
    commands.insert(commands.end(), node.commands.begin(), node.commands.end());
    declarations.insert(declarations.end(), node.declarations.begin(), node.declarations.end());
}

#include "CFGParts.hpp"

using namespace eraxc::CFG;


void CFGNode::operator+=(const CFGNode& node) {
    nodes.insert(nodes.end(), node.nodes.begin(), node.nodes.end());
    declarations.insert(declarations.end(), node.declarations.begin(), node.declarations.end());
}

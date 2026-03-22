// #pragma once
// #include "backend/JIR/Operation.hpp"
// #include "util/common.hpp"
//
// namespace eraxc::JIR::Allocated {
//
//
//     // allocated variable
//     struct OperandAllocated {
//         enum AllocPlace {
//             STACK,
//             REGISTER,
//             GLOBAL,
//             INSTANT
//         };
//         u64 type;
//         u64 value;  //stack offset or register ID
//         AllocPlace place;  //where operand is allocated
//     };
//
//     struct JIRAOp {
//         Operation op = Operation::ERR;
//         OperandAllocated operand1 {};
//         OperandAllocated operand2 {};
//     };
//
//     using Nodes = std::vector<JIRAOp>;
//
//     struct CFGA_Node {
//         Nodes body;
//     };
// }
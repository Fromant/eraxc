#pragma once
#include <sstream>

typedef unsigned long long int u64;
typedef long long int i64;

inline std::string int_to_hex(const size_t i) {
    std::stringstream ss;
    ss << std::hex << i;
    return ss.str();
}

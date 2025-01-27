#ifndef COMPILER_PREPROCESSOR_H
#define COMPILER_PREPROCESSOR_H

#include <fstream>
#include <unordered_map>
#include <sstream>

namespace eraxc {
    static inline std::unordered_map<std::string, std::string> defines{};

    std::string preprocess(const std::string &filename);

    std::string process_macro(std::ifstream &f, const std::string &filename);
}


#endif //COMPILER_PREPROCESSOR_H

//
// Created by frama on 22.10.2024.
//

#ifndef BLCK_COMPILER_BLCK_COMPILER_H
#define BLCK_COMPILER_BLCK_COMPILER_H

#include <string>
#include <fstream>
#include <iostream>

namespace blck {

    class blck_compiler {

        static void compile(const std::string &in_file, const std::string &out_file);

    };

} // blck

#endif //BLCK_COMPILER_BLCK_COMPILER_H

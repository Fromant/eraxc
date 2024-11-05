#include <thread>
#include "blck_compiler.h"
#include "frontend/lexic/preprocessor.h"
#include "frontend/lexic/lexic.h"

namespace blck {

    void blck_compiler::compile(const std::string &in_file, const std::string &out_file) {
        std::string r = preprocess(in_file);

        std::ofstream of(out_file);
        if (!of) {
            std::cerr << "Cannot write to file: " << in_file << '\n';
            exit(-1);
        }
        of << r;

        auto tokens = lexic::tokenize(r, in_file);

    }

    void test(int a) {
        std::thread t{[&]() {

        }};
    }


} // blck
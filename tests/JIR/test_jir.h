#ifndef TEST_JIR_H
#define TEST_JIR_H

#define ALL_TESTS_JIR 1
#include "../../src/backend/JIR/CFG/errors.h"

namespace tests {

    namespace JIR {

        inline error::errable<eraxc::JIR::CFG> open_cfg(const std::string& filename) {
            eraxc::tokenizer tokenizer {};
            auto tokens = tokenizer.tokenize_file("../tests/JIR/files/"+filename);
            if (!tokens) {
                std::cerr << "JIR " << filename << " test failed to tokenize with error:\n" << tokens.error << '\n';
                return {"token_err", {}};
            }
            eraxc::JIR::CFG cfg {};
            auto cfg_err = cfg.create(tokens.value);
            return {cfg_err.error, cfg};
        }

        inline bool global_no_main() {
            const std::string filename = "global_no_main.erx";

            auto cfg = open_cfg(filename);
            if (cfg) { return false; }

            if (cfg.error != eraxc::JIR::NO_ENTRYPOINT_ERROR) {
                std::cerr << "Test JIR " << filename << " failed:\nExpected: " << eraxc::JIR::NO_ENTRYPOINT_ERROR
                          << "\nGot: " << cfg.error << '\n';
                return false;
            }

            if (cfg.value.get_nodes()[0].allocations.size() != 1) return false;

            return true;
        }
    }

    inline int test_jir() {
        int successful_tests = 0;
        if (JIR::global_no_main()) successful_tests++;


        return ALL_TESTS_JIR - successful_tests;
    }

}

#endif

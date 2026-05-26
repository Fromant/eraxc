#include <chrono>
#include <iostream>

#include "src/frontend/lexic/PreprocessorTokenizer.hpp"
#include "src/pipeline.hpp"

int main(int argc, char* argv[]) {
    // auto err = compilation_pipeline("../examples/0.erx");
    // auto err = compilation_pipeline("../examples/loops/while.erx");
    // auto err = compilation_pipeline("../examples/if/if_else.erx");
    // auto err = compilation_pipeline("../examples/if/if.erx");
    // auto err = compilation_pipeline("../tests/files/integration/functions/call.erx");
    // auto err = compilation_pipeline("../tests/files/integration/functions/recursion.erx");
    // auto err = compilation_pipeline("../tests/files/integration/if/nestedIf.erx");
    auto err = compilation_pipeline("../tests/files/integration/if/simpleIf.erx");
    // auto err = compilation_pipeline("../tests/files/integration/weird.erx");
    // auto err = compilation_pipeline("../tests/files/integration/if_diff_stack.erx");
    // auto err = compilation_pipeline("../tests/files/integration/globals_usage.erx");
    // auto err = compilation_pipeline("../tests/files/integration/globals_shared.erx");
    // auto err = compilation_pipeline("../tests/files/integration/globals_init.erx");
    if (!err) {
        std::cerr << err.error << std::endl;
        exit(-1);
    }
    return 0;
}

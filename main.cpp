#include <chrono>
#include <iostream>

#include "src/frontend/lexic/preprocessor_tokenizer.h"
#include "src/pipeline.h"

int main(int argc, char* argv[]) {
    // auto err = compilation_pipeline("../examples/0.erx");
    // auto err = compilation_pipeline("../examples/loops/while.erx");
    // auto err = compilation_pipeline("../examples/if/if_else.erx");
    // auto err = compilation_pipeline("../examples/if/if.erx");
    auto err = compilation_pipeline("../tests/files/integration/call.erx");
    // auto err = compilation_pipeline("../tests/files/integration/weird.erx");
    // auto err = compilation_pipeline("../tests/files/integration/nestedIf.erx");
    // auto err = compilation_pipeline("../tests/files/integration/if_diff_stack.erx");
    if (!err) {
        std::cerr << err.error << std::endl;
        exit(-1);
    }
    return 0;
}

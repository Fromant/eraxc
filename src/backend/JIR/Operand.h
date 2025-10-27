#pragma once

#include "../../util/common.h"

namespace eraxc::JIR {
    struct Operand {
        u64 type;
        u64 value;
        bool is_instant;
        bool is_rvalue;
    };
}

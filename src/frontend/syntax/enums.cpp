
#include "enums.h"

namespace eraxc::syntax {

    const inline std::string empty{};

    const std::string &find_op(eraxc::syntax::operator_type t) {
        if (t == NONE) return empty;
        for (auto &p: operators) {
            if (p.second == t) return p.first;
        }
        for (auto &p: unary_operators) {
            if (p.second == t) return p.first;
        }
        for (auto &p: postfix_operators) {
            if (p.second == t) return p.first;
        }
        return NOT_FOUND;
    }
}

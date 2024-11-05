
#include "enums.h"

namespace blck::syntax {

    const std::string &find_op(blck::syntax::operator_type t) {
        for (auto &p: operators) {
            if (p.second == t) return p.first;
        }
        return NOT_FOUND;
    }
}

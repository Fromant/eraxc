#pragma once

#include <string>
#include <unordered_map>

#include "util/common.h"

namespace eraxc {

    struct Scope {

        struct Declaration {
        private:
            u64 type;
            u64 id;
            bool _isFunc;

        public:
            Declaration(u64 type, u64 id, bool is_func) : type(type), id(id), _isFunc(is_func) {}
            Declaration() : type(-1), id(-1), _isFunc(false) {}

            bool isFunc() const {
                return _isFunc;
            }
            u64 getId() const {
                return id;
            }
            u64 getType() const {
                return type;
            }
            void setId(u64 id) {
                this->id = id;
            }
            bool operator==(const Declaration& declaration) const {
                return type == declaration.type && id == declaration.id && _isFunc == declaration.isFunc();
            };
        };


        //number of allocations (including anonymous)
        u64 allocatedIds = 0;
        std::unordered_map<std::string, Declaration> identifiers {};
        std::unordered_map<std::string, size_t> typenames {};

        Scope() = default;
    };
}

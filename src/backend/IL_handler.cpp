#include "IL_handler.h"


namespace eraxc::IL {


    enum IL_decl::type IL_decl::get_type(u64 type) {
        if (type == syntax::Keyword::u8 || type == syntax::Keyword::i8 || type == syntax::Keyword::CHAR ||
            type == syntax::Keyword::BYTE) {
            return IL_decl::b8;
        } else if (type == syntax::Keyword::u16 || type == syntax::Keyword::i16 || type == syntax::Keyword::SHORT) {
            return IL_decl::b16;
        } else if (type == syntax::Keyword::u32 || type == syntax::Keyword::i32 || type == syntax::Keyword::INT) {
            return IL_decl::b32;
        } else if (type == syntax::Keyword::u64 || type == syntax::Keyword::i64 || type == syntax::Keyword::LONG) {
            return IL_decl::b64;
        }
        return IL_decl::NONE;
    }

    std::vector<IL_node> translate(const syntax::AST::statement_node &statement) {
        return {};
    }

    std::vector<IL_node> translate(const syntax::AST::expr_node &expr) {
        return {};
    }
}
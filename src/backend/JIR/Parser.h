#pragma once

#include <stack>
#include <vector>

#include "Operand.h"
#include "frontend/lexic/preprocessor_tokenizer.h"
#include "frontend/syntax/enums.h"
#include "util/error.h"


namespace eraxc::JIR {
    class Parser {

    public:
        error::errable<std::vector<token>> shuntingYard(const std::vector<token>& tokens) {
            std::stack<token> operands {};
            std::stack<syntax::operator_type> operations {};


            for (const auto& token : tokens) {
                if (token.t == token::INSTANT || token.t == token::STRING_INSTANT) {
                    operands.push(token);
                }
                if (token.t == token::L_BRACKET) {

                }
            }
        }
    };
}

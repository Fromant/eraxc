#pragma once

#include <fstream>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "../../util/error.hpp"

namespace eraxc::frontend {
    struct Token {
        static inline std::set<char> special_symbols {';', '\'', '\"', '=', '+', '<', '>', '%', '/',
                                                      '*', '!',  '&',  '|', '^', '(', ')', '{', '}',
                                                      '[', ']',  '-',  '?', ':', '.', ',', '~'};

        static inline std::set<char> operator_chars {'<', '=', '>', '&', '|', '^', '%',
                                                     '*', '/', '~', '+', '-', '!', '?'};

        static inline std::set<char> instant_number_chars {'u', 'l', 'i', '.', '0', '1', '2',
                                                           '3', '4', '5', '6', '7', '8', '9'};

        enum type {
            SEMICOLON,
            COLON,
            COMMA,
            DOT,
            OPERATOR,
            IDENTIFIER,
            INSTANT,
            STRING_INSTANT,
            L_BRACKET,
            R_BRACKET,
            R_SQ_BRACKET,
            L_SQ_BRACKET,
            R_F_BRACKET,
            L_F_BRACKET,
            NONE,
        };

        type t;
        std::string data;

        Token();
        Token(type t, const std::string& data);

        bool operator==(const Token& other) const;
    };

    struct Tokenizer {
        std::unordered_map<std::string, std::string> defined;

        error::errable<std::vector<Token>> process_macro(std::stringstream& ss);

        static bool is_identifier_char(char c);

        static void add_token(std::vector<Token>& tr, std::stringstream& tmp, Token::type t);

        error::errable<std::vector<Token>> tokenize(std::stringstream& f);

        error::errable<std::vector<Token>> tokenize(const std::string& s) {
            auto ss = std::stringstream {s};
            return tokenize(ss);
        }

        error::errable<std::vector<Token>> tokenize_file(const std::string& filename) {
            std::ifstream f {filename};
            if (!f)
                return {"Cannot open file: " + filename, {}};
            std::stringstream ss;
            ss << f.rdbuf();
            const auto& tokenized = tokenize(ss);
            if (!tokenized) {
                return {tokenized.error, {}};
            }
            return {"", tokenized.value};
        }
    };
}

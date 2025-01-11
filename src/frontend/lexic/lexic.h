#ifndef COMPILER_LEXIC_H
#define COMPILER_LEXIC_H

#include <string>
#include <vector>
#include <sstream>
#include "../../util/error.h"

namespace eraxc::lexic {

    static inline std::string special_symbols{';', '\'', '\"', '=', '+', '<', '>',
                                              '%', '/', '*', '!', '&', '|', '^', '(', ')',
                                              '{', '}', '[', ']', '-', '?', ':', '.', ',', '~'};

    static inline std::vector<std::string> operators{"==", "=", ">", "<", ">=", "<=", "+", "-", "*", "/", "%",
                                                     "&&", "||", "&", "|", "^", "~", ">>", "<<",
                                                     "+=", "-=", "*=", "/=", "%=",
                                                     "&=", "^=", "|=", "~=", ">>=", "<<="};
    static inline std::string operator_chars{"<=>&|^%*/~+-"};

    struct Token {
        enum type {
            SEMICOLON,
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
    };

    static void add_token(std::vector<Token> &tr, std::stringstream &tmp, Token::type &t) {
        if (t != Token::NONE) tr.emplace_back(t, tmp.str());
        tmp.str(std::string());
        t = Token::NONE;
    }

    static std::vector<Token> tokenize(const std::string &s, const std::string &filename) {
        int line = 1;
        std::vector<Token> tr{};
        std::stringstream tmp{};
        Token::type t = Token::NONE;
        for (int i = 0; i < s.size(); i++) {
            char c = s[i];
            if (c == ';') {
                add_token(tr, tmp, t);
                tr.emplace_back(Token::SEMICOLON, ";");
                continue;
            }
            if (c == '\n') {
                add_token(tr, tmp, t);
                line++;
                continue;
            }
            if (c == ' ') {
                add_token(tr, tmp, t);
                continue;
            }
            if (c == '(') {
                add_token(tr, tmp, t);
                tr.emplace_back(Token::L_BRACKET, "(");
                continue;
            }
            if (c == ')') {
                add_token(tr, tmp, t);
                tr.emplace_back(Token::R_BRACKET, ")");
                continue;
            }
            if (c == '{') {
                add_token(tr, tmp, t);
                tr.emplace_back(Token::L_F_BRACKET, "{");
                continue;
            }
            if (c == '}') {
                add_token(tr, tmp, t);
                tr.emplace_back(Token::R_F_BRACKET, "}");
                continue;
            }
            if (c == '[') {
                add_token(tr, tmp, t);
                tr.emplace_back(Token::L_SQ_BRACKET, "[");
                continue;
            }
            if (c == ']') {
                add_token(tr, tmp, t);
                tr.emplace_back(Token::R_SQ_BRACKET, "]");
                continue;
            }
            if (c == '.') {
                add_token(tr, tmp, t);
                tr.emplace_back(Token::DOT, ".");
                continue;
            }
            if (c == ',') {
                add_token(tr, tmp, t);
                tr.emplace_back(Token::COMMA, ",");
                continue;
            }
            if (c == '"') {
                add_token(tr, tmp, t);
                i++;
                while (i < s.size()) {
                    if (s[i] == '\n') break;
                    if (s[i] == '"') {
                        t = Token::STRING_INSTANT;
                        add_token(tr, tmp, t);
                        break;
                    }
                    tmp << s[i];
                    i++;
                }
                if (i == s.size()) {
                    error::fatal(filename, line, "expected '\"' before EOF", 1);
                }
                continue;
            }
            if (operator_chars.find(c) != std::string::npos) {
                //is an operator
                add_token(tr, tmp, t);
                tmp << c;
                while (operator_chars.find(s[i + 1]) != std::string::npos && i + 1 < s.size()) {
                    tmp << s[i];
                    i++;
                }
                t = Token::OPERATOR;
                add_token(tr, tmp, t);
                continue;
            }
            if (std::isdigit(c) && t != Token::IDENTIFIER) {
                t = Token::INSTANT;
            }
            if (std::isprint(c) && t == Token::NONE) t = Token::IDENTIFIER;

            tmp << c;
        }
        tr.emplace_back(Token::NONE, "");
        return tr;
    }
}


#endif //COMPILER_LEXIC_H

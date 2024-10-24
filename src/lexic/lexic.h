//
// Created by frama on 22.10.2024.
//

#ifndef COMPILER_LEXIC_H
#define COMPILER_LEXIC_H

#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>

namespace blck::lexic {

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


        explicit Token(type t, const std::string &data) {
            this->t = t;
            this->data = data;
        }
    };

    static void add_token(std::vector<Token> &tr, std::stringstream &tmp, Token::type &t) {
        if (t != Token::NONE) tr.emplace_back(t, tmp.str());
        tmp.str(std::string());
        t = Token::NONE;
    }

    static std::vector<Token> tokenize(const std::string &s) {
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
            if (c == ' ' || c == '\n') {
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
                    if (s[i] == '"') {
                        t = Token::STRING_INSTANT;
                        add_token(tr, tmp, t);
                        break;
                    }
                    tmp << s[i];
                    i++;
                }
                if (i == s.size()) {
                    //not found
                    //TODO error
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
        //scan till space or operator or endl

        return tr;
    }

    enum operators {
        //arithmetic
        ADD,  //+
        SUBTRACT, //-
        MULTIPLY, //*
        DIVIDE, // /
        MODULO, //%

        //compare
        EQUAL,
        NOT_EQUAL,
        GREATER,
        LESS,
        GREATER_EQ,
        LESS_EQ,

        //logic
        AND,
        OR,
        NOT,
        //bitwise
        BITWISE_NOT,
        BITWISE_AND,
        BITWISE_OR,
        BITWISE_XOR,
        BITWISE_LSHIFT,
        BITWISE_RSHIFT,

        //assign
        ASSIGN,
        ADD_ASSIGN,
        SUBTRACT_ASSIGN,
        MULT_ASSIGN,
        DIV_ASSIGN,
        MOD_ASSIGN,
        BITWISE_AND_ASSIGN,
        BITWISE_OR_ASSIGN,
        BITWISE_XOR_ASSIGN,
        BITWISE_LSHIFT_ASSIGN,
        BITWISE_RSHIFT_ASSIGN,

        //call operator
    };


}


#endif //COMPILER_LEXIC_H

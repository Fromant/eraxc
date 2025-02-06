#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <sstream>
#include <set>

#include "../../util/error.h"

namespace eraxc {
    struct token {
        static inline std::set<char> special_symbols{
            ';', '\'', '\"', '=', '+', '<', '>',
            '%', '/', '*', '!', '&', '|', '^', '(', ')',
            '{', '}', '[', ']', '-', '?', ':', '.', ',', '~'
        };

        static inline std::set<char> operator_chars{'<', '=', '>', '&', '|', '^', '%', '*', '/', '~', '+', '-'};

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

        token() {
            t = NONE;
            data = std::string{};
        }

        token(type t, const std::string& data) {
            this->t = t;
            this->data = data;
        }
    };

    struct tokenizer {
        std::unordered_map<std::string, std::string> defined;

        error::errable<std::vector<token>> process_macro(std::stringstream& ss) {
            std::string macro;
            ss >> macro;
            if (macro == "define") {
                std::string m;
                std::getline(ss, m);
                size_t last = m.find_last_of(' ');
                std::string define = m.substr(0, last);
                std::string to_def = m.substr(last + 1, m.size() - last);
                if (define.empty()) {
                    define = to_def;
                    to_def = "";
                }
                defined[define] = to_def;
                return {"", {}};
            }
            if (macro == "ifdef") {
                std::string def{};
                ss >> def;
                if (defined.contains(def)) {
                    //scroll until #endif
                    std::stringstream tr{};
                    std::string l;
                    do {
                        ss >> l;
                        if (l == "#endif") break;
                        tr << l << ' ';
                    } while (!ss.eof());
                    if (l != "#endif")
                        return {"expected #endif before EOF", {}};
                    return tokenize(tr);
                }
                std::string l;
                do {
                    ss >> l;
                    if (l == "#endif") break;
                } while (!ss.eof());
                if (l != "#endif")
                    return {"expected #endif before EOF: ", {}};
                return {"", {}};
            }
            if (macro == "ifndef") {
                std::string def{};
                ss >> def;
                if (!defined.contains(def)) {
                    //scroll until endif
                    std::stringstream tr{};
                    std::string l;
                    do {
                        ss >> l;
                        if (l == "#endif") break;
                        tr << l << ' ';
                    } while (!ss.eof());
                    if (l != "#endif")
                        return {"expected #endif before EOF: ", {}};
                    return tokenize(tr);
                }
                std::string l;
                do {
                    ss >> l;
                    if (l == "#endif") break;
                } while (!ss.eof());
                if (l != "#endif")
                    return {"expected #endif before EOF: ", {}};
                return {"", {}};
            }
            return {{"No such macro: " + macro},
                {}};
        }

        static void add_token(std::vector<token>& tr, std::stringstream& tmp, token::type t) {
            if (t != token::NONE) tr.emplace_back(t, tmp.str());
            tmp.str(std::string{});
        }

        error::errable<std::vector<token>> tokenize(std::stringstream& f) {
            std::vector<token> tokens;

            char c;
            std::stringstream tmp{};
            while (!f.eof()) {
                f.get(c);
                if (f.eof()) break;
                if (c == ' ' || c == '\n' || c == '\t') {
                    continue;
                }
                if (c == '/' && f.peek() == '/') {
                    //we're in comment, skip line
                    do { f.get(c); } while (c != '\n');
                    continue;
                }
                if (c == '#') {
                    auto r = process_macro(f);
                    if (!r) return {r.error, tokens};
                    tokens.insert(tokens.end(), r.value.cbegin(), r.value.cend());
                    continue;
                }
                if (c == '(') {
                    tokens.emplace_back(token::L_BRACKET, "(");
                    continue;
                }
                if (c == ')') {
                    tokens.emplace_back(token::R_BRACKET, ")");
                    continue;
                }
                if (c == '[') {
                    tokens.emplace_back(token::L_SQ_BRACKET, "[");
                    continue;
                }
                if (c == ']') {
                    tokens.emplace_back(token::R_SQ_BRACKET, "]");
                    continue;
                }
                if (c == '{') {
                    tokens.emplace_back(token::L_F_BRACKET, "{");
                    continue;
                }
                if (c == '}') {
                    tokens.emplace_back(token::R_F_BRACKET, "}");
                    continue;
                }
                if (c == ';') {
                    tokens.emplace_back(token::SEMICOLON, ";");
                    continue;
                }
                if (c == '.') {
                    tokens.emplace_back(token::DOT, ".");
                    continue;
                }
                if (c == ',') {
                    tokens.emplace_back(token::COMMA, ",");
                    continue;
                }
                if (c == '"') {
                    while (!f.eof()) {
                        f.get(c);
                        if (c == '\n')break;
                        if (c == '"') {
                            add_token(tokens, tmp, token::STRING_INSTANT);
                            break;
                        }
                        tmp << c;
                    }
                    if (f.eof()) return {R"(expected end of string instant (""") before EOF)", tokens};
                    continue;
                }
                if (token::operator_chars.contains(c)) {
                    //is an operator
                    tmp << c;
                    while (token::operator_chars.contains(char(f.peek())) && !f.eof()) {
                        tmp << char(f.get());
                    }
                    add_token(tokens, tmp, token::OPERATOR);
                    continue;
                }
                if (std::isdigit(c)) {
                    tmp << c;
                    while (std::isdigit(f.peek()) && !f.eof()) {
                        tmp << char(f.get());
                    }
                    add_token(tokens, tmp, token::INSTANT);
                    continue;
                }
                if (std::isalpha(c)) {
                    tmp << c;
                    while (std::isalpha(f.peek()) && !f.eof()) {
                        tmp << char(f.get());
                    }
                    add_token(tokens, tmp, token::IDENTIFIER);
                    continue;
                }
                tmp << c;
            }


            return {"", tokens};
        }


        error::errable<std::vector<token>> tokenize_file(const std::string& filename) {
            std::ifstream f{filename};
            if (!f) return {"Cannot open file: " + filename, {}};
            std::stringstream ss;
            ss << f.rdbuf();
            return tokenize(ss);
        }
    };
}


#endif //TOKENIZER_H

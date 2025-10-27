
#include "preprocessor_tokenizer.h"

#include <fstream>
#include <limits>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

eraxc::token::token() {
    t = NONE;
    data = std::string {};
}

eraxc::token::token(type t, const std::string& data) {
    this->t = t;
    this->data = data;
}

bool eraxc::token::operator==(const token& other) const {
    return t == other.t && data == other.data;
}

bool eraxc::tokenizer::is_identifier_char(char c) {
    return c == '_' || c == '-' || std::isalpha(c) || std::isdigit(c);
}

error::errable<std::vector<eraxc::token>> eraxc::tokenizer::process_macro(std::stringstream& ss) {

    const auto read_until_endif = [](std::stringstream& ss) -> error::errable<std::string> {
        std::string buffer;
        std::string line;
        int depth = 0;  // current nesting level

        while (std::getline(ss, line)) {
            // Trim leading whitespace to check for directives
            auto start = line.find_first_not_of(" \t");
            if (start == std::string::npos) {
                // Empty or whitespace-only line
                buffer += line + '\n';
                continue;
            }
            std::string trimmed = line.substr(start);

            // Check for preprocessor directives
            if (trimmed.rfind("#ifdef", 0) == 0 || trimmed.rfind("#ifndef", 0) == 0) {
                depth++;
            } else if (trimmed == "#endif") {
                if (depth == 0) {
                    // Found the matching #endif for the original #ifdef
                    return {"", buffer};
                }
                depth--;
            }
            // Append the full line (including original whitespace)
            buffer += line + '\n';
        }

        return {"expected #endif before EOF", {}};
    };

    std::string macro;
    ss >> macro;
    if (macro == "define") {
        char c;
        ss.get(c);  //load first identifier letter
        ss.get(c);  //load first identifier letter
        std::string def;
        if (c != '_' && !std::isalpha(c))
            return {"#define expected identifier as first argument", {}};
        do {
            def += c;
            ss.get(c);
        } while (is_identifier_char(c) && !ss.eof());
        if (c == '\n') {
            defined[def] = "";
            return {"", {}};
        }
        if (c != ' ')
            return {"Expected end of line or space at the end of identifier", {}};
        std::string to_def;
        ss.get(c);
        do {
            if (!is_identifier_char(c))
                return {"Expected identifier as define second argument", {}};
            to_def += c;
            ss.get(c);
        } while (!ss.eof() && c != '\n');
        defined[def] = to_def;
        return {"", {}};
    }
    if (macro == "ifdef") {
        std::string def {};
        ss >> def;
        //skip rest of line
        ss.ignore(std::numeric_limits<int>::max(), '\n');
        if (defined.contains(def)) {
            auto block_result = read_until_endif(ss);
            if (!block_result)
                return {block_result.error, {}};
            std::stringstream tr(block_result.value);
            return tokenize(tr);
        }

        auto skip_result = read_until_endif(ss);
        if (!skip_result)
            return {skip_result.error, {}};
        return {"", {}};
    }
    if (macro == "ifndef") {
        std::string def;
        ss >> def;
        ss.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (!defined.contains(def)) {
            auto block_result = read_until_endif(ss);
            if (!block_result)
                return {block_result.error, {}};
            std::stringstream tr(block_result.value);
            return tokenize(tr);
        }
        auto skip_result = read_until_endif(ss);
        if (!skip_result)
            return {skip_result.error, {}};
        return {"", {}};
    }
    return {{"No such macro: " + macro}, {}};
}

void eraxc::tokenizer::add_token(std::vector<token>& tr, std::stringstream& tmp, token::type t) {
    if (t != token::NONE)
        tr.emplace_back(t, tmp.str());
    tmp.str(std::string {});
}

error::errable<std::vector<eraxc::token>> eraxc::tokenizer::tokenize(std::stringstream& f) {
    std::vector<token> tokens;

    char c;
    std::stringstream tmp {};
    while (!f.eof()) {
        f.get(c);
        if (f.eof())
            break;
        if (c == ' ' || c == '\n' || c == '\t') {
            continue;
        }
        if (c == '/' && f.peek() == '/') {
            //we're in comment, skip line
            do {
                f.get(c);
            } while (c != '\n' && !f.eof());
            continue;
        }
        if (c == '#') {
            auto r = process_macro(f);
            if (!r)
                return {r.error, tokens};
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
        if (c == ':') {
            tokens.emplace_back(token::COLON, ":");
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
                // if (c == '\n')
                // break;
                if (c == '"') {
                    add_token(tokens, tmp, token::STRING_INSTANT);
                    break;
                }
                tmp << c;
            }
            if (f.eof())
                return {R"(expected end of string instant (""") before EOF)", tokens};
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
            while ((std::isdigit(f.peek()) || token::instant_number_chars.contains(f.peek()) || f.peek() == '.') &&
                   !f.eof()) {
                tmp << char(f.get());
            }
            add_token(tokens, tmp, token::INSTANT);
            continue;
        }
        if (std::isalpha(c) || c == '_') {
            tmp << c;
            while (is_identifier_char(f.peek()) && !f.eof()) {
                tmp << char(f.get());
            }
            if (defined.contains(tmp.str()) && !defined.at(tmp.str()).empty())
                tmp.str(defined.at(tmp.str()));
            add_token(tokens, tmp, token::IDENTIFIER);
            continue;
        }
        tmp << c;
    }


    return {"", tokens};
}

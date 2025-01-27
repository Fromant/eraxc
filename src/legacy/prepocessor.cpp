#include "preprocessor.h"
#include "../util/error.h"

namespace eraxc {
    std::string preprocess_lines(std::stringstream &tp, const std::string &filename);

    std::string process_macro(std::stringstream &ss, const std::string &filename) {
        std::string macro;
        ss >> macro;
        if (macro == "include") {
            std::string fname;
            if (ss.get() != ' ') error::fatal(filename, 0, "expected filename in quotes after include", -2);
            if (ss.get() != '\"') error::fatal(filename, 0, "expected filename in quotes after include", -2);
            ss >> fname;
            if (fname[fname.length() - 1] != '\"')
                error::fatal(filename, 0, "expected filename in quotes after include: ", -2);
            fname.pop_back();
            if (fname == filename) error::fatal(filename, 0, "cannot include cycling file", -3);
            return preprocess(fname);
        }
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

            defines[define] = to_def;
            return {};
        }
        if (macro == "ifdef") {
            std::string def{};
            ss >> def;
            if (defines.contains(def)) {
                //scroll until endif
                std::stringstream tr{};
                std::string l;
                do {
                    ss >> l;
                    if (l == "#endif") break;
                    tr << l << ' ';
                } while (!ss.eof());
                if (l != "#endif") {
                    error::fatal("", 0, "expected #endif before EOF: ", -1);
                }
                return preprocess_lines(tr, filename);
            } else {
                std::string l;
                do {
                    ss >> l;
                    if (l == "#endif") break;
                } while (!ss.eof());
                if (l != "#endif") {
                    error::fatal("", 0, "expected #endif before EOF: ", -1);
                }
                return {};
            }
        }
        if (macro == "ifndef") {
            std::string def{};
            ss >> def;
            if (!defines.contains(def)) {
                //scroll until endif
                std::stringstream tr{};
                std::string l;
                do {
                    ss >> l;
                    if (l == "#endif") break;
                    tr << l << ' ';
                } while (!ss.eof());
                if (l != "#endif") {
                    error::fatal("", 0, "expected #endif before EOF: ", -1);
                }
                return preprocess_lines(tr, filename);
            } else {
                std::string l;
                do {
                    ss >> l;
                    if (l == "#endif") break;
                } while (!ss.eof());
                if (l != "#endif") {
                    error::fatal("", 0, "expected #endif before EOF: ", -1);
                }
                return {};
            }
        }
        error::fatal("", 0, "No such macro: " + macro, -1);
        return macro;
    }

    std::string preprocess_lines(std::stringstream &tp, const std::string &filename) {
        bool was_last_space = false;
        bool was_last_slash = false;
        std::stringstream ss{};
        while (true) {
            char c;
            tp.get(c);
            if (tp.eof()) break;
            if (c == ' ' || c == '\t' || c == '\n') {
                if (was_last_space) continue;
                else was_last_space = true;
            } else was_last_space = false;

            if (c == '/') {
                if (was_last_slash) {
                    tp.get(c);
                    while (c != '\n' && !tp.eof()) {
                        tp.get(c);
                    }
                    was_last_slash = false;
                } else was_last_slash = true;
                continue;
            } else {
                if (was_last_slash) ss << '/';
                was_last_slash = false;
            }

            if (c == '#') {
                //process macro
                ss << process_macro(tp, filename);
                continue;
            }
            ss.put((char) c);
        }
        ss << '\n';
        return ss.str();
    }

    std::string preprocess(const std::string &filename) {
        std::ifstream f(filename, std::ios::in);
        if (!f) {
            error::fatal(filename, 0, "cannot open file", -1);
        }
        std::stringstream alls{};
        alls << f.rdbuf();
        f.close();
        auto processed = preprocess_lines(alls, filename);
        return processed;
    }
}
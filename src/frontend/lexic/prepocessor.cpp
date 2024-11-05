#include "preprocessor.h"
#include "../../util/error.h"

namespace blck {
    std::unordered_map<std::string, std::string> defines{};

    std::string process_macro(std::ifstream &f, const std::string &filename) {
        std::string macro;
        f >> macro;
        if (macro == "include") {
            std::string fname;

            return preprocess(fname);
        }
        if (macro == "define") {
            std::string m;
            std::getline(f, m);
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
            f >> def;
            if (defines.contains(def)) {
                //scroll until endif
                std::stringstream tr{};
                std::string l;
                do {
                    f >> l;
                    if (l == "#endif") break;
                    tr << l << ' ';
                } while (!f.eof());
                if (l != "#endif") {
                    error::fatal(filename, 0, "expected #endif before EOF: ", -1);
                }
                return tr.str();
            } else {
                std::string l;
                do {
                    f >> l;
                    if (l == "#endif") break;
                } while (!f.eof());
                if (l != "#endif") {
                    error::fatal(filename, 0, "expected #endif before EOF: ", -1);
                }
                return {};
            }
        }
        if (macro == "ifndef") {
            std::string def{};
            f >> def;
            if (!defines.contains(def)) {
                //scroll until endif
                std::stringstream tr{};
                std::string l;
                do {
                    f >> l;
                    if (l == "#endif") break;
                    tr << l;
                } while (f.eof());
                if (l != "#endif") {
                    error::fatal(filename, 0, "expected #endif before EOF: ", -1);
                }
                return tr.str();
            } else {
                std::string l;
                do {
                    f >> l;
                    if (l == "#endif") break;
                } while (!f.eof());
                if (l != "#endif") {
                    error::fatal(filename, 0, "expected #endif before EOF: ", -1);
                }
                return {};
            }
        }
        error::fatal(filename, 0, "No such macro: " + macro, -1);
        return macro;
    }

    std::string preprocess(const std::string &filename) {
        std::ifstream f(filename, std::ios::in);
        if (!f) {
            error::fatal(filename, 0, "cannot open file", - 1);
        }
        std::stringstream ss{};
        int c;
        bool was_last_space = false;
        bool was_last_slash = false;
        while ((c = f.get()) != EOF) {
            if (c == ' ' || c == '\t' || c == '\n') {
                if (was_last_space) continue;
                else was_last_space = true;
            } else was_last_space = false;
            if (c == '/') {
                if (was_last_slash) {
                    while (c != '\n') c = f.get();
                    was_last_slash = false;
                } else was_last_slash = true;
                continue;
            } else {
                if (was_last_slash) ss << '/';
                was_last_slash = false;
            }
            if (c == '#') {
                //process macro
                ss << process_macro(f, filename);
                continue;
            }
            ss.put((char) c);
        }

        return ss.str();
    }
}
#include "./error.h"

#include <iostream>

void error::fatal(const std::string &filename, int line, const std::string &what, int err_code) {
    std::cerr << "FATAL ERROR in " << filename << ':' << line << " : " << what << '\n';
    exit(err_code);
}

void error::critical(const std::string &filename, int line, const std::string &what, int err_code) {
    std::cerr << "CRITICAL ERROR in " << filename << ':' << line << " : " << what << '\n';
}

void error::warning(const std::string &filename, int line, const std::string &what, int err_code) {
    std::clog << "WARNING in " << filename << ':' << line << " : " << what << '\n';
}
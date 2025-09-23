#ifndef ERRORS_H
#define ERRORS_H
#include <string>

namespace eraxc::JIR {
    const std::string NO_ENTRYPOINT_ERROR = "Entrypoint `int main() {...}` not found!";
    const std::string UNKNOWN_TYPENAME_ERROR = "Unknown typename: ";
    const std::string UNKNOWN_IDENTIFIER_ERROR = "Unknown identifier: ";

}

#endif  //ERRORS_H

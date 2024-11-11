#ifndef ERAXC_ERROR_H
#define ERAXC_ERROR_H

#include <string>

namespace error {

    template<typename v>
    struct errable {
        std::string error;
        v value;

        errable(const char *error, v val) {
            this->error = std::string{error};
            value = val;
        }

        errable(const std::string &error, const v &val) {
            this->error = std::string{error};
            value = val;
        }

        explicit errable(v val) {
            error;
            value = val;
        }

        explicit operator bool() {
            return error.empty();
        }
    };

    template<>
    struct errable<void> {
        std::string error;

        errable(const char *error) {
            this->error = std::string{error};
        }

        errable(const std::string &error) {
            this->error = std::string{error};
        }

        explicit operator bool() const {
            return error.empty();
        }
    };

    /// Fatal error stops the process of compilation of file
    /// \param what error to print
    void fatal(const std::string &filename, int line, const std::string &what, int err_code);

    /// Critical error does not exits the process of compilation
    /// \param what error to print
    void critical(const std::string &filename, int line, const std::string &what, int err_code);

    /// Just warn user
    /// \param what error to print
    void warning(const std::string &filename, int line, const std::string &what, int err_code);
}

#endif //ERAXC_ERROR_H

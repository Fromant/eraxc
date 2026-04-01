#pragma once

#include <string>

namespace error {

    template<typename T>
    struct errable {
        std::string error;
        T value;

        errable(const char* error, const T& val) : error(error), value(val) {}
        errable(const char* error, T&& val) : error(error), value(val) {}

        errable(std::string error, const T& val) : error(std::move(error)), value(val) {}
        errable(std::string error, T&& val) : error(std::move(error)), value(val) {}

        explicit errable(const T& val) {
            error = "";
            value = val;
        }

        explicit operator bool() const {
            return error.empty();
        }
    };

    template<>
    struct errable<void> {
        std::string error;

        errable(const char* error) {
            this->error = std::string {error};
        }

        errable(const std::string& error) {
            this->error = std::string {error};
        }

        explicit operator bool() const {
            return error.empty();
        }
    };

    /// Fatal error stops the process of compilation of file
    /// \param what error to print
    void fatal(const std::string& filename, int line, const std::string& what, int err_code);

    /// Critical error does not exits the process of compilation
    /// \param what error to print
    void critical(const std::string& filename, int line, const std::string& what, int err_code);

    /// Just warn user
    /// \param what error to print
    void warning(const std::string& filename, int line, const std::string& what, int err_code);
}

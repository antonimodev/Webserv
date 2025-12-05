#pragma once

#include <stdexcept>
#include <string>

/**
 * @brief Exception for parsing errors, with custom message
 */
class ParseException : public std::runtime_error {
    private:
        std::string _msg;

    public:
        ParseException(const std::string& msg);

        virtual ~ParseException(void) throw();

        virtual const char* what(void) const throw();
};
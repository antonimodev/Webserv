#pragma once

#include <stdexcept>
#include <string>


class ParseException : public std::runtime_error {
    public:
        ParseException(const std::string& msg) : std::runtime_error(msg) {}
};
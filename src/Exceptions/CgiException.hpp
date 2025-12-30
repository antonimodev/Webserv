#pragma once

#include <stdexcept>


class CgiException : public std::runtime_error {
    public:
        explicit CgiException(const std::string& msg) : std::runtime_error(msg) {}
};
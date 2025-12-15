#pragma once

#include <stdexcept>


class PendingRequestException : public std::runtime_error {
    public:
        explicit PendingRequestException(const std::string& msg) : std::runtime_error(msg) {}
};
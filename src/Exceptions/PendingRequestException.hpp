#pragma once

#include <stdexcept>


class PendingRequestException : public std::runtime_error {
    public:
        PendingRequestException(const std::string& msg) : std::runtime_error(msg) {}
};
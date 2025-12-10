#pragma once

#include <stdexcept>
#include <string>


class SocketException : public std::runtime_error {
	public:
		explicit SocketException(const std::string& msg) : std::runtime_error(msg) {}
};
#pragma once

#include <exception>
#include <string>

class SocketException : public std::runtime_error {
	public:
		SocketException(const std::string& msg) : std::runtime_error(msg) {}
};
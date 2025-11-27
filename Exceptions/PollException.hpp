#pragma once

#include <stdexcept>
#include <string>


class PollException : public std::runtime_error {
	public:
		PollException(const std::string& msg) : std::runtime_error(msg) {}
};
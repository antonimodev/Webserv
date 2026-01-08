#pragma once

#include <stdexcept>


class PollException : public std::runtime_error {
	public:
		explicit PollException(const std::string& msg) : std::runtime_error(msg) {}
};

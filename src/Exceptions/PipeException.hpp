#pragma once

#include <stdexcept>


class PipeException : public std::runtime_error {
	public:
		explicit PipeException(const std::string& msg) : std::runtime_error(msg) {}
};

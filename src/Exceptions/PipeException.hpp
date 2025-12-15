#pragma once

#include <stdexcept>
#include <string>


class PipeException : public std::runtime_error {
	public:
		explicit PipeException(const std::string& msg) : std::runtime_error(msg) {}
};
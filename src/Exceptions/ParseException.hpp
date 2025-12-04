#pragma once

#include <exception>

class OpenFailed : public std::exception {
	public:
		const char* what() const throw() {
			return "Error: opening conf file failed.";
		}
};
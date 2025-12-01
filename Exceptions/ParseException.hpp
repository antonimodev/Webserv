#pragma once

#include <stdexcept>
#include <string>
#include <sstream>

enum HttpStatus {
	BAD_REQUEST = 400,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	METHOD_NOT_ALLOWED = 405,
	INTERNAL_ERROR = 500
};

class ParseException : public std::runtime_error {
	private:
		HttpStatus _code;

		static const char* statusToString(const HttpStatus& status);

	public:
		ParseException(const std::string& msg);
		ParseException(const HttpStatus& status);

		std::string httpResponse() const;
};
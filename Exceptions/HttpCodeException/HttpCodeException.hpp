#pragma once

#include <stdexcept>
#include <string>

enum HttpStatus {
	BAD_REQUEST = 400,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	METHOD_NOT_ALLOWED = 405,
	INTERNAL_ERROR = 500
};

class HttpCodeException : public std::runtime_error {
	private:
		const char*	statusToString(const HttpStatus& status) const;
		HttpStatus			_status;

	public:
		HttpCodeException(const HttpStatus& code, const std::string& log_msg);

		std::string			httpResponse(void) const;
};
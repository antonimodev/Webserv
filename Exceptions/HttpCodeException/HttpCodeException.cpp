#include "HttpCodeException.hpp"

HttpCodeException::HttpCodeException(const HttpStatus& code) : std::runtime_error(statusToString(code)) {}

// PRIVATE
const char*	HttpCodeException::statusToString(const HttpStatus& status) const {
	switch (status) {
		case BAD_REQUEST:           return "400 Bad Request";
		case FORBIDDEN:             return "403 Forbidden";
		case NOT_FOUND:             return "404 Not Found";
		case METHOD_NOT_ALLOWED:    return "405 Method Not Allowed";
		case INTERNAL_ERROR:        return "500 Internal Server Error";
		default:					return "DEFAULT";
	}
}
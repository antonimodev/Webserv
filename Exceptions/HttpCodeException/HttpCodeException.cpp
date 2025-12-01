#include "HttpCodeException.hpp"
#include <sstream>

HttpCodeException::HttpCodeException(const HttpStatus& code, const std::string& log_msg)
	: std::runtime_error(log_msg), _status(code) {}

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

// PUBLIC

std::string HttpCodeException::httpResponse(void) const {
	const char* status = statusToString(_status);

	std::ostringstream oss;
	oss << "<html><body><h1>" << status << "</h1></body></html>";
	std::string body = oss.str();

	std::ostringstream response;
	response << "HTTP/1.1 " << status << "\r\n"
				<< "Content-Type: text/html\r\n"
				<< "Content-Length: " << body.size() << "\r\n"
				<< "\r\n" << body;
	return response.str();
}
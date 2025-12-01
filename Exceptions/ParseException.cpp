#include <sstream>

#include "ParseException.hpp"

ParseException::ParseException(const std::string& msg) : std::runtime_error(msg) {}

ParseException::ParseException(const HttpStatus& code) : std::runtime_error(statusToString(code)), _code(code) {}

// PRIVATE
const char* ParseException::statusToString(const HttpStatus& status) {
    switch (status) {
        case BAD_REQUEST:           return "400 Bad Request";
        case FORBIDDEN:             return "403 Forbidden";
        case NOT_FOUND:             return "404 Not Found";
        case METHOD_NOT_ALLOWED:    return "405 Method Not Allowed";
        case INTERNAL_ERROR:        return "500 Internal Server Error";
    }
    return "500 Internal Server Error";
}


std::string ParseException::httpResponse() const {
    const char* status = statusToString(_code);
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

#include "HttpCodeException.hpp"
#include <sstream>
#include "webserv.h"


HttpCodeException::HttpCodeException(HttpStatus code, const std::string& log_msg)
	: std::runtime_error(log_msg), _status(code) {}


// PRIVATE
const char* HttpCodeException::statusToString(HttpStatus status) const {
	switch (status) {
		case BAD_REQUEST:        return "400 Bad Request";
		case FORBIDDEN:          return "403 Forbidden";
		case NOT_FOUND:          return "404 Not Found";
		case METHOD_NOT_ALLOWED: return "405 Method Not Allowed";
		case INTERNAL_ERROR:     return "500 Internal Server Error";
		default:                 return "500 Internal Server Error";
	}
}


// PUBLIC
std::string HttpCodeException::httpResponse() const {
	const char* status = statusToString(_status);

	std::ostringstream oss;
	oss << "<html><body><h1>" << status << "</h1></body></html>";
	std::string body = oss.str();

	std::ostringstream response;
	response	<< "HTTP/1.1 " << status << "\r\n"
				<< "Content-Type: text/html\r\n"
				<< "Content-Length: " << body.size() << "\r\n"
				<< "\r\n" << body;

	return response.str();
}


/*

std::string HttpCodeException::httpResponse(const ServerConfig* config) const {
    const char* status = statusToString(_status);
    std::string body;

    // --------- search for error page in ServerConfig
    if (config) {
        std::map<int, std::string>::const_iterator it = config->error_pages.find(_status);
        if (it != config->error_pages.end()) {
            try {
                std::string error_page_path = config->root + it->second;
                body = get_file_content(error_page_path);
            } catch (...) {
                // Si falla cargar la página personalizada, usar HTML genérico
                body = "";
            }
        }
    }

    // ------- generic HTML if no custom page loaded
    if (body.empty()) {
        std::ostringstream oss;
        oss << "<html><body><h1>" << status << "</h1></body></html>";
        body = oss.str();
    }

	// ------- set the requested html body to the response
    std::ostringstream response;
    response	<< "HTTP/1.1 " << status << "\r\n"
                << "Content-Type: text/html\r\n"
                << "Content-Length: " << body.size() << "\r\n"
                << "\r\n" << body;

    return response.str();
}

*/
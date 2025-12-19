#include "HttpCodeException.hpp"
#include <sstream>
#include "webserv.h"
#include <iostream>


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

std::string HttpCodeException::getErrorHtml(const ServerConfig* config) const {
	std::map<int, std::string>::const_iterator it = config->error_pages.find(_status);
	if (it != config->error_pages.end()) {
		try {
			std::string error_page_path = config->root + it->second;
			return get_file_content(error_page_path);
		} catch (const std::exception& e) { // this fails if the file cannot be opened/read
			std::cerr << "Error loading custom error page: " << e.what() << std::endl;
		}
	}
	return "";
}

static std::string getDefaultErrorHtml(const char* status) {
        std::ostringstream oss;
        oss << "<html><body><h1>" << status << "</h1></body></html>";
        return oss.str();
}

// PUBLIC
std::string HttpCodeException::httpResponse(const ServerConfig* config) const {
    const char* status = statusToString(_status);
    std::string body;

    // --------- search for _status in error_pages map in ServerConfig
    if (config)
		body = getErrorHtml(config);

    // ------- generic HTML if no custom page loaded on server config
    if (body.empty())
		body = getDefaultErrorHtml(status);

	// ------- set the requested html body to the response
    std::ostringstream response;

    response	<< "HTTP/1.1 " << status << "\r\n"
                << "Content-Type: text/html\r\n"
                << "Content-Length: " << body.size() << "\r\n"
                << "\r\n" << body;

    return response.str();
}


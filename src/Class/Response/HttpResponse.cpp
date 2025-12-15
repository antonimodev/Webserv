#include "HttpResponse.hpp"
#include <stdexcept>
#include <sstream>

HttpResponse::HttpResponse() : _status_code(0), _body("") {}

HttpResponse::~HttpResponse() {}

void HttpResponse::setStatus(int code) {
	if (code < 100 || code > 504)
		throw std::invalid_argument("Invalid HTTP status code");
	_status_code = code;
}

void HttpResponse::setHeader(const std::string &key, const std::string &value) {
	_headers[key] = value;
}

void HttpResponse::setBody(const std::string &body) {
	_body = body;
}

std::string HttpResponse::getStatusMessage(int code) const {
    static std::map<int, std::string> messages;
    
    if (messages.empty()) {

        messages[100] = "Continue";
        messages[101] = "Switching Protocols";
        messages[102] = "Processing";

        messages[200] = "OK";
        messages[201] = "Created";
        messages[204] = "No Content";
        
		messages[301] = "Moved Permanently";
        messages[302] = "Found";
        messages[304] = "Not modified";
		
		messages[400] = "Bad Request";
        messages[401] = "Unauthorized";
        messages[403] = "Forbidden";
		messages[404] = "Not Found";
        messages[429] = "Too Many Requests";

		messages[500] = "Internal Server Error";
        messages[502] = "Bad Gateway";
        messages[503] = "Service Unavailable";
		messages[504] = "Gateway Timeout";
    }
    return messages[code];
}

std::string HttpResponse::toString() const {
    std::string response;
	std::stringstream ss;

	ss << _status_code;
	response += "HTTP/1.1 " + ss.str() + " " + getStatusMessage(_status_code) + "\r\n";
    
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it) {
    	response += it->first + ": " + it->second + "\r\n";
	}
    
    response += "\r\n";

	response += _body;
    
    return response;
}
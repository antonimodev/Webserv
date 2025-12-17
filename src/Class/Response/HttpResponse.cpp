#include "HttpResponse.hpp"
#include <stdexcept>
#include <sstream>

HttpResponse::HttpResponse() : _status_code(0), _body("") {
    _messages[100] = "Continue";
    _messages[101] = "Switching Protocols";
    _messages[102] = "Processing";
    _messages[200] = "OK";
    _messages[201] = "Created";
    _messages[204] = "No Content";
    _messages[301] = "Moved Permanently";
    _messages[302] = "Found";
    _messages[304] = "Not modified";
    _messages[400] = "Bad Request";
    _messages[401] = "Unauthorized";
    _messages[403] = "Forbidden";
    _messages[404] = "Not Found";
    _messages[429] = "Too Many Requests";
    _messages[500] = "Internal Server Error";
    _messages[502] = "Bad Gateway";
    _messages[503] = "Service Unavailable";
    _messages[504] = "Gateway Timeout";
}

HttpResponse::~HttpResponse() {}

void HttpResponse::setStatus(int code) {
	_status_code = code;
}

void HttpResponse::setHeader(const std::string &key, const std::string &value) {
	_headers[key] = value;
}

void HttpResponse::setBody(const std::string &body) {
	_body = body;
}

std::string HttpResponse::getStatusMessage(int code) const {
    std::map<int, std::string>::const_iterator it = _messages.find(code);
    if (it == _messages.end())
        throw std::invalid_argument("Invalid HTTP status code");
    return it->second;
}

std::string HttpResponse::toString() const {
    std::string response;
	std::stringstream ss;

	ss << _status_code;
	response += "HTTP/1.1 " + ss.str() + " " + getStatusMessage(_status_code) + "\r\n";
    
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it) {
    	response += it->first + ": " + it->second + "\r\n";
	}

    ss.str("");
    ss.clear();
    ss << _body.size();
    response += "Content-Length: " + ss.str() + "\r\n";

    response += "\r\n";

	response += _body;
    
    return response;
}
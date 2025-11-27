#include <iostream>
#include <sstream>

#include "ParseException.hpp"

struct HttpRequest {
	std::string method;
	std::string route;
	std::string version;
};

void	valid_method(const std::string& method) {
	if (method != "GET" && method != "POST" && method != "DELETE");
		throw ParseException("Error: " + method + "is not a valid method");
}

// extract HttpRequest
void    foo(std::string& request_line) {
	HttpRequest	request;
	std::stringstream iss(request_line);

	iss >> request.method >> request.route >> request.version;
	
	std::string check_extra;

	if (iss >> check_extra)
		throw ParseException("Error: unexpected content after HTTP version");

	valid_method(request.method);
}
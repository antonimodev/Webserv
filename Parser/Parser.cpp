#include <iostream>
#include <sstream>
#include <unistd.h>

#include "HttpCodeException.hpp"
#include "Parser.hpp"

// CONSTRUCTORS
Parser::Parser(void) {}

Parser::~Parser(void) {}


// PRIVATE FUNCTIONS
void	Parser::validMethod(const std::string& method) {
	if (method != "GET" && method != "POST" && method != "DELETE")
		throw HttpCodeException(BAD_REQUEST, "Error: " + method + " is not a valid method");
}

void	Parser::validRoute(const std::string& route) {
	if (route[0] != '/')
		throw HttpCodeException(BAD_REQUEST, "Error: route must start with '/'");
	
	if (route.find("..") != std::string::npos)
		throw HttpCodeException(BAD_REQUEST, "Error: path traversal not allowed");

	for (size_t i = 0; i < route.size(); ++i) {
		if (!std::isgraph(route[i]))
			throw HttpCodeException(BAD_REQUEST, "Error: invalid character in route");
	}
}

void	Parser::validVersion(const std::string& version) {
	if (version != "HTTP/1.1")
		throw HttpCodeException(BAD_REQUEST, "Error: " + version + " not valid");
}


void	Parser::validHttpRequest(const std::string& request, HttpRequest& httpStruct) {
	std::stringstream iss(request);

	if (!(iss >> httpStruct.method >> httpStruct.route >> httpStruct.version))
		throw HttpCodeException(BAD_REQUEST, "Error: malformed request line");

	// Probably we'll have to check only first line
/* 	std::string check_extra;

	if (iss >> check_extra)
		throw HttpCodeException("Error: unexpected content after HTTP version"); */

	validMethod(httpStruct.method);
	validRoute(httpStruct.route);
	validVersion(httpStruct.version);
}

// extract HttpRequest
HttpRequest    Parser::parseHttpRequest(const std::string& request) {
	//throw HttpCodeException(BAD_REQUEST, "For testing purposes"); // test
	HttpRequest httpStruct;

	validHttpRequest(request, httpStruct);
	return httpStruct;
}

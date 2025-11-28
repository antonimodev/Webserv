#include <iostream>
#include <sstream>
#include <unistd.h>

#include "ParseException.hpp"
#include "Parser.hpp"

// CONSTRUCTORS
Parser::Parser(void) {}

Parser::~Parser(void) {}


// PRIVATE FUNCTIONS
void	Parser::validMethod(const std::string& method) {
	if (method != "GET" && method != "POST" && method != "DELETE")
		throw ParseException("Error: " + method + " is not a valid method");
}

void	Parser::validRoute(const std::string& route) {
	if (route[0] != '/')
		throw ParseException("Error: route must start with '/'");
	
	if (route.find("..") != std::string::npos)
		throw ParseException("Error: path traversal not allowed");

	for (size_t i = 0; i < route.size(); ++i) {
		if (!std::isgraph(route[i]))
			throw ParseException("Error: invalid character in route");
	}
}

void	Parser::validVersion(const std::string& version) {
	if (version != "HTTP/1.1")
		throw ParseException("Error: " + version + " not valid");
}

void	Parser::validHttpRequest(const std::string& request, HttpRequest& httpStruct) {
	std::stringstream iss(request);

	if (!(iss >> httpStruct.method >> httpStruct.route >> httpStruct.version))
		throw ParseException("Error: malformed request line");
	
	std::string check_extra;

	if (iss >> check_extra)
		throw ParseException("Error: unexpected content after HTTP version");

	validMethod(httpStruct.method);
	validRoute(httpStruct.route);
	validVersion(httpStruct.version);
}

// extract HttpRequest
HttpRequest    Parser::parseHttpRequest(const std::string& request) {
	HttpRequest httpStruct;

	validHttpRequest(request, httpStruct);
	return httpStruct;
}










/* void	Parser::parseRequestLine(const std::string& line, HttpRequest& req) {
	std::istringstream iss(line);
	std::string extra;

	if (!(iss >> req.method >> req.route >> req.version))
		throw ParseException("Error: malformed request line");

	if (iss >> extra)
		throw ParseException("Error: unexpected content after HTTP version");
}

void	Parser::validateRequest(const HttpRequest& req) {
	validMethod(req.method);
	validRoute(req.route);
	validVersion(req.version);
}

// PUBLIC FUNCTIONS
HttpRequest	Parser::parseHttpRequest(const std::string& request) {
	HttpRequest req;

	std::istringstream iss(request);
	std::string request_line;
	std::getline(iss, request_line);

	parseRequestLine(request_line, req);
	validateRequest(req);

	return req;
} */
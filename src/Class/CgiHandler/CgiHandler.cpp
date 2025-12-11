#include <iostream>

#include "webserv.h"
#include "CgiHandler.hpp"
#include "Parser.hpp"


CgiHandler::CgiHandler(const HttpRequest& request, const std::string& full_path) {
	_env["REQUEST_METHOD"] = request.method;
	_env["QUERY_STRING"] = request.query;
	_env["PATH_INFO"] = request.route;
	_env["SCRIPT_FILENAME"] = full_path;
	_env["CONTENT_LENGTH"] = getHeader(request, "Content-Length");
	_env["CONTENT_TYPE"] = getHeader(request, "Content-Type");

	_env["GATEWAY_INTERFACE"] = "CGI/1.1"; // RFC (Request For Comments) good practice
	_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	_env["REDIRECT_STATUS"] = "200";
}


CgiHandler::CgiHandler(const CgiHandler& other) {
	(void)other;
}


CgiHandler& CgiHandler::operator=(const CgiHandler& other) {
	(void)other;
	return *this;
}


CgiHandler::~CgiHandler(void) {
	// TODO
}


// PRIVATE


std::string CgiHandler::getHeader(const HttpRequest& request, const std::string& key) {
	std::map<std::string, std::string>::const_iterator it = request.headers.find(key);

	if (it == request.headers.end())
		return "";

	return it->second;
}


//
int execve(const char *pathname, char *const argv[], char *const envp[]);


void	foo(const std::string& extension) {
	if (extension == "php")
		// pathname = /usr/bin/php-cgi
	else if (extension == "py")
		// /usr/bin/python
	else
		throw();

	//fork()
	// pid == 0? child
		//execve(pathname.c_str(), _ , _);
}
/* 
pathname: depends on extension of script would execute with different binary
		-> /usr/bin/php-cgi
		-> /usr/bin/python


argv:


*/
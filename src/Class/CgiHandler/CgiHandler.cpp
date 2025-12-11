#include <iostream>
#include <unistd.h>

#include "webserv.h"
#include "CgiHandler.hpp"
#include "ExecveBuilder.hpp"
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


CgiHandler::~CgiHandler(void) {}


// PRIVATE


std::string CgiHandler::getHeader(const HttpRequest& request, const std::string& key) {
	std::map<std::string, std::string>::const_iterator it = request.headers.find(key);

	if (it == request.headers.end())
		return "";

	return it->second;
}

//

void	CgiHandler::executeCgi(const std::string& extension, const std::string& script) {
	std::vector<std::string> args;

	if (extension == "php")
		args.push_back("/usr/bin/php-cgi");
	else
		args.push_back("/usr/bin/python3");

	args.push_back(script);

	ExecveBuilder	env_builder(_env); // receives map
	ExecveBuilder	arg_builder(args); // receives vector

	//fork()

	execve(args[0].c_str(), arg_builder.get(), env_builder.get());
}

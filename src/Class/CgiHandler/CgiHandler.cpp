#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

#include "webserv.h"
#include "CgiHandler.hpp"
#include "ExecveBuilder.hpp"
#include "Parser.hpp"
#include "Pipe.hpp"

#include "PipeException.hpp"


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


// PUBLIC

// objective: [0] /usr/bin/php-cgi [1] script.php [2] NULL
void	CgiHandler::executeCgi(const HttpRequest& request) {
	std::vector<std::string> args;

	if (get_extension(request.route) == "php")
		args.push_back("/usr/bin/php-cgi");
	else
		args.push_back("/usr/bin/python3");

	args.push_back(request.route);

	ExecveBuilder	env_builder(_env); // receives map
	ExecveBuilder	arg_builder(args); // receives vector
	
	try {
		Pipe pipes;
		int status;

		pid_t child = fork();

		if (child == 0)
			execve(args[0].c_str(), arg_builder.get(), env_builder.get());
		else
			waitpid(child, &status, 0); // just for testing, wouldn't be block
	} catch (const PipeException& e) {
		std::cerr << e.what() << std::cout; // not sure about this catch
		// return ¿? shouldn't continue (STILL IN DEVELOPMENT)
	}
}

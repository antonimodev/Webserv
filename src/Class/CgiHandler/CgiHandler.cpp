#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <sstream>

#include "webserv.h"
#include "CgiHandler.hpp"
#include "ExecveBuilder.hpp"
#include "Parser.hpp"
#include "Pipe.hpp"

#include "PipeException.hpp"
#include "HttpCodeException.hpp"


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
// STATIC JUST FOR TESTING
static std::string process_response(const std::string& content) {
	size_t	header_end = content.find("\r\n\r\n");
	size_t	delimiter_length = 4;

	if (header_end == std::string::npos) {
		header_end = content.find("\n\n");
		delimiter_length = 2;
		if (header_end == std::string::npos)
			throw HttpCodeException(INTERNAL_ERROR, "Error: Invalid CGI response");
	}

	std::string cgi_headers = content.substr(0, header_end);
	std::string cgi_body = content.substr(header_end + delimiter_length);

	std::ostringstream oss;
	oss << "HTTP/1.1 200 OK\r\n"
		<< cgi_headers << "\r\n"
		<< "Content-Length: " << cgi_body.size() << "\r\n"
		<< "\r\n"
		<< cgi_body;

	return oss.str();
}

// objective: [0] /usr/bin/php-cgi [1] script.php [2] NULL
// probably use route instead send httpRequest
std::string	CgiHandler::executeCgi(void) {
	std::vector<std::string> args;

	if (get_extension(_env["PATH_INFO"]) == "php")
		args.push_back("/usr/bin/php-cgi");
	else
		args.push_back("/usr/bin/python3");

	args.push_back(_env["SCRIPT_FILENAME"]);

	ExecveBuilder	env_builder(_env); // receives map
	ExecveBuilder	arg_builder(args); // receives vector
	
	try {
		Pipe pipes;
		int status;

		pid_t child = fork();

		if (child == 0) {
			pipes.fdRedirection(STDOUT_FILENO, Pipe::WRITE);
			execve(args[0].c_str(), arg_builder.get(), env_builder.get());
		}

		pipes.closeWritePipe();
		std::string response = process_response(get_fd_content(pipes.getReadPipe()));
		waitpid(child, &status, 0); // Should stay here?

		return response;
	} catch (const PipeException& e) {
		std::cerr << e.what() << std::cout; // not sure about this catch
		// return ¿? shouldn't continue (STILL IN DEVELOPMENT)
	}

	return ""; // we may move return response right here to avoid return empty string
}
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
std::string	CgiHandler::executeCgi(const HttpRequest& request) {
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

		if (child == 0) {
			pipes.fdRedirection(STDOUT_FILENO, WRITE);
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
}

// HTTP/1.1 200 OK\r\n
// Content-Length: X\r\n

// SCRIPT: Content-Type: text/html (e.g) \r\n

// \r\n

// SCRIPT:<h1>Hello World!</h1>

std::string process_response(const std::string& content) {
	size_t header_end = content.find("\r\n\r\n");
	// may we should use find("\n\n"), it depends of Operative system

	if (header_end == std::string::npos)
		throw std::runtime_error("Invalid CGI response"); // runtime error till find appropiate exception

	std::string cgi_headers = content.substr(0, header_end);
	std::string cgi_body = content.substr(header_end + 4);

	std::ostringstream oss;
	oss << "HTTP/1.1 200 OK\r\n"
		<< cgi_headers << "\r\n"
		<< "Content-Length: " << cgi_body.size() << "\r\n"
		<< "\r\n"
		<< cgi_body;

	return oss.str();
}
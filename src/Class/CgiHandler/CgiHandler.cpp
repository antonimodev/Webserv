#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <sstream>

#include "webserv.h"
#include "CgiHandler.hpp"
#include "ExecveBuilder.hpp"
#include "Parser.hpp"
#include "Pipe.hpp"

#include "CgiException.hpp"
#include "PipeException.hpp"
#include "HttpCodeException.hpp"


CgiHandler::CgiHandler(const HttpRequest& request, const std::string& full_path, std::pair<std::string, std::string> cgi_extension) {
	_body = request.body;

    _env["REQUEST_METHOD"] = request.method;
    _env["QUERY_STRING"] = request.query;

    _env["PATH_INFO"] = "";
    _env["SCRIPT_FILENAME"] = full_path;
    setScriptInfo(request.route, full_path, cgi_extension.first);

    _env["CONTENT_LENGTH"] = getHeader(request, "Content-Length");
    _env["CONTENT_TYPE"] = getHeader(request, "Content-Type");

    _env["GATEWAY_INTERFACE"] = "CGI/1.1";
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


void	CgiHandler::setScriptInfo(const std::string& route, const std::string& full_path, const std::string& cgi_ext) {
    size_t pos = route.find(cgi_ext);
    
    if (pos != std::string::npos) {
        size_t end_of_script = pos + cgi_ext.size();

        _env["PATH_INFO"] = route.substr(end_of_script);

        size_t path_info_len = _env["PATH_INFO"].size();

        if (full_path.size() >= path_info_len) {
            _env["SCRIPT_FILENAME"] = full_path.substr(0, full_path.size() - path_info_len);
        }
    }
}


// PUBLIC

std::string CgiHandler::process_response(const std::string& content) {
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


int CgiHandler::executeCgi(pid_t& pid, std::pair<std::string, std::string>& cgi_extension) {
    std::vector<std::string> args;

    args.push_back(cgi_extension.second);
    args.push_back(_env["SCRIPT_FILENAME"]);

    ExecveBuilder env_builder(_env);
    ExecveBuilder arg_builder(args);

    try {
        Pipe output_pipe;		// from child (script output) to parent
        Pipe input_pipe;		// from parent (script input) to child

        pid_t child = fork();
        pid = child;

        if (child == 0) {
            output_pipe.fdRedirection(STDOUT_FILENO, Pipe::WRITE);
            input_pipe.fdRedirection(STDIN_FILENO, Pipe::READ);

            execve(args[0].c_str(), arg_builder.get(), env_builder.get());
            throw CgiException("Error: execve failed");
        }

        output_pipe.closeWritePipe();
        input_pipe.closeReadPipe();

        if (!_body.empty()) {
            write(input_pipe.getWritePipe(), _body.c_str(), _body.size());
        }

        input_pipe.closeWritePipe();

        return output_pipe.fdRelease(Pipe::READ);

    } catch (const PipeException& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
}

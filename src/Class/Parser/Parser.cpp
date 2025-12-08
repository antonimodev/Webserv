#include <iostream>
#include <sstream>

#include "Parser.hpp"
#include "webserv.h"

#include "HttpCodeException.hpp"
#include "PendingRequestException.hpp"


Parser::Parser(void) {}


Parser::Parser(const Parser& other) {
	(void)other;
}


Parser& Parser::operator=(const Parser& other) {
	(void)other;
	return *this;
}


Parser::~Parser(void) {}


// PRIVATE - REQUEST LINE VALIDATION


void	Parser::validMethod(const std::string& method) {
	if (method != "GET" && method != "POST" && method != "DELETE")
		throw HttpCodeException(BAD_REQUEST, "Error: " + method + " is not a valid method");
}


void	Parser::validRoute(const std::string& route) {
	if (route.empty() || route[0] != '/')
		throw HttpCodeException(BAD_REQUEST, "Error: route must start with '/'");

	if (route.find("..") != std::string::npos)
		throw HttpCodeException(BAD_REQUEST, "Error: path traversal not allowed");

	for (size_t i = 0; i < route.size(); ++i) {
		if (!std::isgraph(static_cast<unsigned char>(route[i])))
			throw HttpCodeException(BAD_REQUEST, "Error: invalid character in route");
	}
}


void	Parser::validVersion(const std::string& version) {
	if (version != "HTTP/1.1")
		throw HttpCodeException(BAD_REQUEST, "Error: " + version + " not valid");
}


// PRIVATE - PARSING


/**
 * @brief Parses the first line: "METHOD ROUTE HTTP/1.1\r\n"
 * @param request Full request string.
 * @param http_struct Output struct to populate.
 * @param pos In/Out position, updated to after "\r\n".
 * @complexity O(n) where n is length of request line.
 */
void	Parser::parseRequestLine(const std::string& request, HttpRequest& http_struct, size_t& pos) {
	size_t endl = request.find("\r\n", pos);
	if (endl == std::string::npos)
		throw HttpCodeException(BAD_REQUEST, "Error: malformed request line");

	std::string request_line = request.substr(pos, endl - pos); // use pos for generic purposes
	std::istringstream iss(request_line);
	std::string extra;

	if (!(iss >> http_struct.method >> http_struct.route >> http_struct.version))
		throw HttpCodeException(BAD_REQUEST, "Error: malformed request line");

	if (iss >> extra)
		throw HttpCodeException(BAD_REQUEST, "Error: malformed request line");

	validMethod(http_struct.method);
	validRoute(http_struct.route);
	validVersion(http_struct.version);

	pos = endl + 2;
}


/**
 * @brief Parses headers until empty line "\r\n\r\n".
 * @param request Full request string.
 * @param http_struct Output struct to populate headers map.
 * @param pos In/Out position, updated to start of body.
 * @complexity O(n) where n is total length of headers.
 */
void	Parser::parseHeaders(const std::string& request, HttpRequest& http_struct, size_t& pos) {
	while (pos < request.size()) {
		size_t endl = request.find("\r\n", pos);
		if (endl == std::string::npos)
			break;

		if (endl == pos) {
			pos += 2;  // Skip "\r\n"
			return;
		}

		std::string header_line = request.substr(pos, endl - pos);
		size_t colon_pos = header_line.find(':');

		if (colon_pos == std::string::npos)
			throw HttpCodeException(BAD_REQUEST, "Error: malformed header line");

		std::string key = trim_space(header_line.substr(0, colon_pos));
		std::string value = trim_space(header_line.substr(colon_pos + 1));

		if (key.empty())
			throw HttpCodeException(BAD_REQUEST, "Error: empty header key");

		if (key.find(' ') != std::string::npos)
			throw HttpCodeException(BAD_REQUEST, "Error: header name cannot contain spaces");

		http_struct.headers[key] = value;
		pos = endl + 2;
	}
}


/**
 * @brief Extracts body based on Content-Length header.
 * @param request Full request string.
 * @param http_struct Struct with headers already parsed.
 * @param pos Starting position of body.
 * @complexity O(n) where n is body length.
 */
void	Parser::parseBody(const std::string& request, HttpRequest& http_struct, size_t pos) {
	std::map<std::string, std::string>::const_iterator it = http_struct.headers.find("Content-Length");

	if (it == http_struct.headers.end()) {
		return; // No body expected
	}

	std::istringstream iss(it->second);
	size_t content_length = 0;
	iss >> content_length;

	if (iss.fail())
		throw HttpCodeException(BAD_REQUEST, "Error: invalid Content-Length");

	if (pos + content_length > request.size())
		throw PendingRequestException("Info: waiting to complete request body reading");

	http_struct.body = request.substr(pos, content_length);
}


// PUBLIC


HttpRequest Parser::parseHttpRequest(const std::string& request) {
	HttpRequest http_struct;
	size_t pos = 0;

	parseRequestLine(request, http_struct, pos);
	parseHeaders(request, http_struct, pos);
	parseBody(request, http_struct, pos);

	return http_struct;
}


// ANOTHER FUNCTIONS
const std::string	get_mime_type(const std::string& extension) {
	if (extension == "html") return "text/html";
	if (extension == "css")  return "text/css";
	if (extension == "png")  return "image/png";
	if (extension == "jpg" || extension == "jpeg") return "image/jpeg";
	if (extension == "ico")  return "image/x-icon";
	if (extension == "txt")  return "text/plain";

	return "text/html";
}
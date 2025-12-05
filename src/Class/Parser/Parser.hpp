#pragma once

#include <string>
#include <map>

/**
 * @struct HttpRequest
 * @brief Holds parsed components of an HTTP request.
 */
struct HttpRequest {
	// START_LINE, EXAMPLE: GET /index.html HTTP/1.1
	std::string method;
	std::string route;
	std::string version;

	// HEADERS
	std::map<std::string, std::string> headers; // e.g headers["Content-Length"] = "500";

	// BODY (Content to POST (images, text...))
	std::string body;

};


class Parser {
	private:
		Parser(void);
		Parser(const Parser& other);
		Parser& operator=(const Parser& other);
		~Parser(void);

		// Request Line validation
		static void validMethod(const std::string& method);
		static void validRoute(const std::string& route);
		static void validVersion(const std::string& version);

		// Parsing stages
		static void parseRequestLine(const std::string& request, HttpRequest& http_struct, size_t& pos);
		static void parseHeaders(const std::string& request, HttpRequest& http_struct, size_t& pos);
		static void parseBody(const std::string& request, HttpRequest& http_struct, size_t pos);

	public:
		/**
		 * @brief Parses a raw HTTP request string into HttpRequest struct.
		 * @param request Raw HTTP request as received from client.
		 * @return Populated HttpRequest structure.
		 * @throws HttpCodeException on malformed request.
		 */
		static HttpRequest parseHttpRequest(const std::string& request);
};

const std::string get_mime_type(const std::string& extension);
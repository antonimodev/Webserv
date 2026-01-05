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
	std::string route; 		// e.g	/script.php
	std::string	query;		// e.g	user=antonimo&id=42
	std::string version;

	// HEADERS
	std::map<std::string, std::string> headers; // e.g headers["Content-Length"] = "500";

	// BODY (Content to POST (images, text...))
	std::string body;
};


class Parser {
	private:
		Parser(void);
		Parser(const Parser&);
		Parser& operator=(const Parser&);
		~Parser(void);

		// Request Line validation
		/**
		 * @brief Validates if the HTTP method is GET, POST, or DELETE.
		 * 
		 * This function checks if the provided HTTP method is one of the allowed methods.
		 * 
		 * @param method HTTP method to validate.
		 * @throw HttpCodeException if the method is invalid.
		 */
		static void validMethod(const std::string& method);

		/**
		 * @brief Validates the route for correctness.
		 * 
		 * This function ensures that the provided route is valid and adheres to the expected format.
		 * 
		 * @param route Route to validate.
		 * @throw HttpCodeException if the route is invalid.
		 */
		static void validRoute(const std::string& route);

		/**
		 * @brief Validates if the HTTP version is HTTP/1.1.
		 * 
		 * This function checks if the provided HTTP version matches the supported version (HTTP/1.1).
		 * 
		 * @param version HTTP version to validate.
		 * @throw HttpCodeException if the version is different from HTTP/1.1.
		 */
		static void validVersion(const std::string& version);

		/**
		 * @brief Extracts the route from a full route string (excluding the query string).
		 * 
		 * This function parses the full route string and returns the route without the query string.
		 * 
		 * @param full_route Full route string including the query string.
		 * @return std::string Extracted route.
		 */
		static std::string extractRoute(const std::string& full_route);

		/**
		 * @brief Extracts the query string from a full route string.
		 * 
		 * This function parses the full route string and returns the query string.
		 * 
		 * @param full_route Full route string including the query string.
		 * @return std::string Extracted query string.
		 */
		static std::string extractQuery(const std::string& full_route);

		/**
		 * @brief Parses the request line (e.g., "METHOD ROUTE HTTP/1.1\r\n").
		 * 
		 * This function extracts the method, route, and HTTP version from the request line
		 * and populates the provided HttpRequest structure.
		 * 
		 * @param request Full HTTP request string.
		 * @param http_struct Output structure to populate with parsed data.
		 * @param pos In/Out position, updated to the position after "\r\n".
		 * @complexity O(n), where n is the length of the request line.
		 */
		static void parseRequestLine(const std::string& request, HttpRequest& http_struct, size_t& pos);

		/**
		 * @brief Parses HTTP headers from the request string.
		 * 
		 * This function reads the headers from the HTTP request string and populates
		 * the headers map in the provided HttpRequest structure.
		 * 
		 * @param request Full HTTP request string.
		 * @param http_struct Output structure to populate with headers.
		 * @param pos In/Out position, updated to the start of the body.
		 * @complexity O(n), where n is the total length of the headers.
		 */
		static void parseHeaders(const std::string& request, HttpRequest& http_struct, size_t& pos);

		/**
		 * @brief Extracts the body of the HTTP request based on the Content-Length header.
		 * 
		 * This function reads the body of the HTTP request and populates the body field
		 * in the provided HttpRequest structure.
		 * 
		 * @param request Full HTTP request string.
		 * @param http_struct Structure with headers already parsed.
		 * @param pos Starting position of the body.
		 * @complexity O(n), where n is the length of the body.
		 */
		static void parseBody(const std::string& request, HttpRequest& http_struct, size_t pos);

		/**
		 * @brief Parses a chunked transfer-encoded body.
		 * 
		 * This function processes the chunked transfer-encoded body from the HTTP request
		 * and populates the body field in the provided HttpRequest structure.
		 * 
		 * @param http_struct Structure to populate with the body.
		 * @param request Full HTTP request string.
		 * @param pos In/Out position, updated as chunks are read.
		 * @complexity O(n), where n is the total length of the body.
		 */
		static void parseChunkedBody(HttpRequest& http_struct, const std::string& request, size_t& pos);

		/**
		 * @brief Converts a hexadecimal string to a decimal size_t value.
		 * 
		 * This function takes a hexadecimal string and converts it to its decimal equivalent.
		 * 
		 * @param hex Hexadecimal string to convert.
		 * @return size_t Decimal representation of the hexadecimal value.
		 */
		static size_t hexToDecimal(const std::string& hex);

	public:
		/**
		 * @brief Parses a raw HTTP request string into an HttpRequest structure.
		 * 
		 * This function processes the raw HTTP request string, extracting the request line,
		 * headers, and body, and populates an HttpRequest structure with the parsed data.
		 * 
		 * @param request Raw HTTP request string as received from the client.
		 * @return HttpRequest Populated structure with parsed HTTP request data.
		 * @throws HttpCodeException if the request is malformed.
		 */
		static HttpRequest parseHttpRequest(const std::string& request);
};
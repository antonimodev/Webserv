#pragma once

#include <stdexcept>
#include <string>
#include "ConfParser.hpp"

/**
 * @brief HTTP status codes for error responses.
 */
enum HttpStatus {
	BAD_REQUEST = 400,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	METHOD_NOT_ALLOWED = 405,
	INTERNAL_ERROR = 500
};


/**
 * @brief Exception representing an HTTP error response.
 * 
 * Carries both a log message and the ability to generate
 * a complete HTTP response string.
 */
class HttpCodeException : public std::runtime_error {
	private:
		HttpStatus _status;

		/**
		 * @brief Converts status code to HTTP status line text.
		 * @param status HTTP status code.
		 * @return C-string with status code and reason phrase.
		 */
		const char* statusToString(HttpStatus status) const;

		/**
		 * @brief Generates the HTML body for the error response.
		 * @param config Server configuration for custom error pages.
		 * @return HTML string for the error response body.
		 */
		std::string getErrorHtml(const ServerConfig* config) const;

	public:
		/**
		 * @brief Constructs an HTTP exception.
		 * @param code HTTP status code.
		 * @param log_msg Message for logging/debugging.
		 */
		HttpCodeException(HttpStatus code, const std::string& log_msg);

		/**
		 * @brief Generates a complete HTTP error response.
		 * @return HTTP response string ready to send to client.
		 */
		std::string	httpResponse(const ServerConfig* config) const;
};
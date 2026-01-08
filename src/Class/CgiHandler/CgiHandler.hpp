#pragma once

#include <map>
#include <string>

#include "Parser.hpp"


// CGI -> Common Gateway Interface

class CgiHandler {
	private:
		std::map<std::string, std::string>  _env;
        std::string                         _body;

		CgiHandler(const CgiHandler&);
		CgiHandler& operator=(const CgiHandler&);

		/**
		 * @brief Retrieves the value of a specific header from an HTTP request.
		 * 
		 * This function searches for the specified header key in the HTTP request
		 * and returns its corresponding value.
		 * 
		 * @param request The HTTP request object containing headers.
		 * @param key The key of the header to retrieve.
		 * @return std::string The value of the specified header, or an empty string if not found.
		 */
		std::string getHeader(const HttpRequest& request, const std::string& key);

		/**
		 * @brief Sets the script information for a CGI request.
		 * 
		 * This function configures the script-related information, including the route,
		 * full path, and CGI extension, for processing a CGI request.
		 * 
		 * @param route The route of the CGI script.
		 * @param full_path The full path to the CGI script.
		 * @param cgi_ext The CGI extension of the script.
		 */
		void        setScriptInfo(const std::string& route, const std::string& full_path, const std::string& cgi_ext);

	public:
		/**
		 * @brief Constructs a new CgiHandler object and sets up CGI environment variables.
		 * 
		 * This constructor initializes the CGI handler with the given HTTP request,
		 * full path to the script, and CGI extension. It also sets up the necessary
		 * environment variables for the CGI process.
		 * 
		 * @param request The HTTP request object.
		 * @param full_path The full path to the CGI script.
		 * @param cgi_extension A pair containing the CGI extension and its interpreter path.
		 */
		CgiHandler(const HttpRequest& request, const std::string& full_path, std::pair<std::string, std::string> cgi_extension);
		~CgiHandler(void);

		/**
		* @brief Executes a CGI script in a child process.
		* 
		* This function forks a new process to execute the CGI script using the
		* appropriate interpreter based on the CGI extension. It sets up the
		* environment variables and handles the execution of the CGI program.
		* 
		* @param pid Reference to store the process ID of the child process.
		* @param cgi_extension Pair containing the CGI extension and its interpreter path.
		* @return int File descriptor for reading the CGI output, or -1 on error.
		*/
		int executeCgi(pid_t& pid, std::pair<std::string, std::string>& cgi_extension);

		/**
		 * @brief Processes the CGI response content.
		 * 
		 * This function processes the raw output from the CGI script, separating
		 * headers from the body and formatting it appropriately for an HTTP response.
		 * 
		 * @param content Raw output from the CGI script.
		 * @return std::string Formatted HTTP response content.
		 */
		static std::string  process_response(const std::string& content);
};

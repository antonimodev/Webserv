#pragma once

#include <map>
#include <string>

#include "Parser.hpp"

// CGI -> Common Gateway Interface

class CgiHandler {
	private:
		std::map<std::string, std::string>  _env;

		CgiHandler(const CgiHandler&);
		CgiHandler& operator=(const CgiHandler&);

		/**
		 * @brief Get the Header object
		 * 
		 * @param request 
		 * @param key 
		 * @return std::string 
		 */
		std::string getHeader(const HttpRequest& request, const std::string& key);

		/**
		 * @brief Set the Script Info object
		 * 
		 * @param route 
		 * @param full_path 
		 * @param cgi_ext 
		 */
		void        setScriptInfo(const std::string& route, const std::string& full_path, const std::string& cgi_ext);

	public:
		/**
		 * @brief Construct a new Cgi Handler object. Also sets up CGI environment variables.
		 * 
		 * @param request 
		 * @param full_path 
		 * @param cgi_extension 
		 */
		CgiHandler(const HttpRequest& request, const std::string& full_path, std::pair<std::string, std::string> cgi_extension);
		~CgiHandler(void);

		/**
		* @brief Executes a CGI script in a child process
		* 
		* This function forks a new process to execute the CGI script using the
		* appropriate interpreter based on the CGI extension. It sets up the
		* environment variables and handles the execution of the CGI program.
		* 
		* @param pid Reference to store the process ID of the child process
		* @param cgi_extension Pair containing the CGI extension and its interpreter path
		* @return int File descriptor for reading the CGI output, or -1 on error
		*/
		int executeCgi(pid_t& pid, std::pair<std::string, std::string>& cgi_extension);

		/**
		 * @brief Process the CGI response content
		 * 
		 * This function processes the raw output from the CGI script, separating
		 * headers from the body and formatting it appropriately for HTTP response.
		 * 
		 * @param content Raw output from the CGI script
		 * @return std::string Formatted HTTP response content
		 */
		static std::string  process_response(const std::string& content);
};
#pragma once

#include <sstream>
#include <stack>
#include <vector>
#include <map>
#include <string>


/**
 * @brief Configuration for a location block
 */
struct LocationConfig {
	std::string							path;
	std::vector<std::string>        	allowed_methods;
	std::string 				   		index;
	std::string                     	root;
	std::string                     	upload_path;
	bool                            	autoindex;
	std::map<std::string, std::string>	cgi_extensions;
	std::pair<int, std::string>			redirect;
	
	LocationConfig(void);
	LocationConfig(std::string path);
};

/**
 * @brief Configuration for a server block
 */
struct ServerConfig {
	std::vector<std::string>				server_names; // changed to vector
	int										listen_port;
	std::string								host;
	std::string								root;
	std::string								index;
	size_t									client_max_body_size;
	std::map<int, std::string>				error_pages;
	std::map<std::string, LocationConfig>	locations;

	ServerConfig(void);
};


/*****************************************************************************
*        HELPER FUNCTIONS (handlers doesn't need object state)               *
*****************************************************************************/

// Server handlers
void	handleServerName(const std::string& value, ServerConfig& config);
void	handleListen(const std::string& value, ServerConfig& config);
void	handleHost(const std::string& value, ServerConfig& config);
void	handleRoot(const std::string& value, ServerConfig& config);
void	handleIndex(const std::string& value, ServerConfig& config);
void	handleClientMaxBodySize(const std::string& value, ServerConfig& config);
void	handleErrorPage(const std::string& value, ServerConfig& config);

// Location handlers
void	handleAllowedMethods(const std::string& value, LocationConfig& location);
void	handleLocationRoot(const std::string& value, LocationConfig& location);
void	handleLocationIndex(const std::string& value, LocationConfig& location);
void	handleUploadPath(const std::string& value, LocationConfig& location);
void	handleAutoindex(const std::string& value, LocationConfig& location);
void	handleCgiExtension(const std::string& value, LocationConfig& location);
void	handleReturn(const std::string& value, LocationConfig& location);


/**
 * @brief Parser for NGINX-style configuration files
 */

class ConfParser {
	private:
		std::stack<std::string>		_brackets;
		std::vector<ServerConfig>	_servers;
		std::string					_current_location_path;

		typedef void	(*ServerHandler)(const std::string&, ServerConfig&);
		typedef void	(*LocationHandler)(const std::string&, LocationConfig&);

		std::map<std::string, ServerHandler>	_serverHandlers;
		std::map<std::string, LocationHandler>	_locationHandlers;
		
		void	initHandlers(void);
		/**
		 * @brief Converts raw content into a vector of tokens.
		 * @param buffer The configuration file content.
		 * @return Vector of parsed tokens.
		 */
		std::vector<std::string> tokenizeContent(const std::string& buffer);
		/**
		 * @brief Processes a single token and applies appropriate handler.
		 * @param token The current token to parse.
		 * @param content The complete token vector for context.
		 * @param i Reference to current position in token vector.
		 */
		void	parseToken(const std::string& token, const std::vector<std::string>& content, size_t& i);
		/**
		 * @brief Stores a token value and advances to next directive.
		 * @param token The token to store.
		 * @param content The complete token vector for context.
		 * @param i Reference to current position in token vector.
		 */
		void	saveToken(std::string token, const std::vector<std::string>& content, size_t& i);
		/**
		 * @brief Validates all parsed server configurations.
		 * @throws ParseException if validation fails.
		 */
		void	validateServers(void);

	public:
		ConfParser(void);

		/**
		 * @brief Constructs and parses a configuration file.
		 * @param fileName Path to the configuration file.
		 * @throws ParseException if file cannot be opened or parsed.
		 */
		ConfParser(const char* fileName);
		~ConfParser(void);
		ConfParser(const ConfParser& other);
		ConfParser& operator=(const ConfParser& other);

		/**
		 * @brief Parses a configuration file.
		 * @param fileName Path to the configuration file.
		 * @throws ParseException if file cannot be opened or parsed.
		 */
		void	parseFile(const char* fileName);
		/**
		 * @brief Returns the parsed server configurations.
		 * @return Constant reference to the vector of ServerConfig structures.
		 */
		const std::vector<ServerConfig>& getServers(void) const;
		void	printServers(void);
};

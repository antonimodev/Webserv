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
	std::vector<std::string>        allowed_methods;
	std::string                     root;
	std::string                     upload_path;
	bool                            autoindex;
	std::pair<std::string, std::string> cgi_extension;
	std::pair<int, std::string>     redirect;
	
	LocationConfig(void);
};

/**
 * @brief Configuration for a server block
 */
struct ServerConfig {
	std::string                     server_name;
	int                             listen_port;
	std::string                     host;
	std::string                     root;
	std::string                     index;
	size_t                          client_max_body_size;
	std::map<int, std::string>      error_pages;
	std::map<std::string, LocationConfig> locations;

	ServerConfig(void);
};


// ════════════════════════════════════════════════════════════════════
// HELPER FUNCTIONS (handlers doesn't need object state)
// ════════════════════════════════════════════════════════════════════

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
void	handleUploadPath(const std::string& value, LocationConfig& location);
void	handleAutoindex(const std::string& value, LocationConfig& location);
void	handleCgiExtension(const std::string& value, LocationConfig& location);
void	handleReturn(const std::string& value, LocationConfig& location);

// Parsing helpers
std::vector<std::string> tokenizeContent(const std::string& buffer);

/**
 * @brief Parser for NGINX-style configuration files
 */
class ConfParser {
	private:
		std::stack<std::string>     _brackets;
		std::vector<ServerConfig>   _servers;
		std::string                 _current_location_path;

		// Typedefs for handlers (now points to helper functions)
		typedef void	(*ServerHandler)(const std::string&, ServerConfig&);
		typedef void	(*LocationHandler)(const std::string&, LocationConfig&);

		// Maps for handlers
		std::map<std::string, ServerHandler>   _serverHandlers;
		std::map<std::string, LocationHandler> _locationHandlers;
		
		void	initHandlers(void);
		std::vector<std::string> tokenizeContent(const std::string& buffer);
		void	parseToken(const std::string& token, const std::vector<std::string>& content, size_t& i);
		void	saveToken(std::string token, const std::vector<std::string>& content, size_t& i);
		void	validateServers(void);


	public:
		ConfParser(void);
		ConfParser(const char* fileName);
		~ConfParser(void);
		
		ConfParser(const ConfParser& other);
		ConfParser& operator=(const ConfParser& other);

		void	parseFile(const char* fileName);
		const std::vector<ServerConfig>& getServers(void) const;

		void	printServers(void);
};
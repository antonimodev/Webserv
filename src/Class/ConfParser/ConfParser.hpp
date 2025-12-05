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

/**
 * @brief Parser for NGINX-style configuration files
 */
class ConfParser {
    private:
        std::stack<std::string>     _brackets;
        std::vector<ServerConfig>   _servers;
        
        // Non-static typedefs
        typedef void (*ServerHandler)(const std::string&, ServerConfig&);
        typedef void (*LocationHandler)(const std::string&, LocationConfig&);
        
        // Non-static maps (per instance)
        std::map<std::string, ServerHandler>   _serverHandlers;
        std::map<std::string, LocationHandler> _locationHandlers;
        
        // Initialization helper
        void initHandlers(void);
        
        // Server handlers
        static void handleServerName(const std::string& value, ServerConfig& config);
        static void handleListen(const std::string& value, ServerConfig& config);
        static void handleHost(const std::string& value, ServerConfig& config);
        static void handleRoot(const std::string& value, ServerConfig& config);
        static void handleIndex(const std::string& value, ServerConfig& config);
        static void handleClientMaxBodySize(const std::string& value, ServerConfig& config);
        static void handleErrorPage(const std::string& value, ServerConfig& config);
        
        // Location handlers
        static void handleAllowedMethods(const std::string& value, LocationConfig& location);
        static void handleLocationRoot(const std::string& value, LocationConfig& location);
        static void handleUploadPath(const std::string& value, LocationConfig& location);
        static void handleAutoindex(const std::string& value, LocationConfig& location);
        static void handleCgiExtension(const std::string& value, LocationConfig& location);
        static void handleReturn(const std::string& value, LocationConfig& location);
        
        // Parsing helpers
		void parseToken(const std::string& token, const std::vector<std::string>& content, size_t& i);
		std::string getValue(const std::vector<std::string>& content, size_t& i);

    public:
        ConfParser(void);
        ConfParser(const char* fileName);
        ~ConfParser(void);
        
        ConfParser(const ConfParser& other);
        ConfParser& operator=(const ConfParser& other);
        
        /**
         * @brief Parse configuration file and store in internal state
         * @param fileName Path to .conf file
         * @throws OpenFailed if file cannot be opened
         */
        void parseFile(const char* fileName);
        
        /**
         * @brief Get parsed server configurations
         * @return Const reference to vector of ServerConfig
         */
        const std::vector<ServerConfig>& getServers(void) const;
};
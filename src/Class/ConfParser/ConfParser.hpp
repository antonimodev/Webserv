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

struct HttpMethods {
    static bool isValid(const std::string& method) {
        return method == "GET" ||
               method == "POST" ||
               method == "DELETE" ||
               method == "PUT";
    }
};

/**
 * @brief Parser for NGINX-style configuration files
 */
class ConfParser {
    private:
	    // ══════════════════════════════════════════════
        // ESTADO DEL OBJETO (necesita acceso con 'this')
        // ══════════════════════════════════════════════
        std::stack<std::string>     _brackets;
        std::vector<ServerConfig>   _servers;
		std::string                 _current_location_path;
        
        // Non-static typedefs
        typedef void (*ServerHandler)(const std::string&, ServerConfig&);
        typedef void (*LocationHandler)(const std::string&, LocationConfig&);
        
        // Non-static maps (per instance)
        std::map<std::string, ServerHandler>   _serverHandlers;
        std::map<std::string, LocationHandler> _locationHandlers;
        
        // Initialization helper
        void initHandlers(void);
        
		// ════════════════════════════════════════════════════════════════════════════════════════════
        // MÉTODOS STATIC (NO usan estado del objeto), tambien los typedefs necesitan que sean static (?)
        // ════════════════════════════════════════════════════════════════════════════════════════════
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
		static std::vector<std::string> tokenizeContent(const std::string& buffer);

		// ══════════════════════════════════════════════
        // MÉTODOS NO-STATIC (usan '_servers', etc.)
        // ══════════════════════════════════════════════
		void parseToken(const std::string& token, const std::vector<std::string>& content, size_t& i);
		std::string getValue(const std::vector<std::string>& content, size_t& i);

    public:
        ConfParser(void);
        ConfParser(const char* fileName);
        ~ConfParser(void);
        
        ConfParser(const ConfParser& other);
        ConfParser& operator=(const ConfParser& other);
        
        void parseFile(const char* fileName);
		void validateServers(void);
        const std::vector<ServerConfig>& getServers(void) const;
		
		void printServers(void);
};
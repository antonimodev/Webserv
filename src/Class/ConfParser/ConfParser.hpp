#pragma once

#include <sstream>

struct ServerConfig {
    std::string server_name;
    int listen_port;
    std::string host;
    std::string root;
    std::string index;
    size_t client_max_body_size;
    std::map<int, std::string> error_pages; // Código de error -> Ruta
    std::map<std::string, std::string> locations; // Ruta -> Configuración
};

class ConfParser {
    private:
        static std::stringstream readContent(const char* fileName);
        static std::map<std::string, void(*)(const std::string&, ServerConfig&)> tokenHandlers;

        // static void handleServerName(const std::string& value, ServerConfig& config);
        // static void handleListen(const std::string& value, ServerConfig& config);
        // static void handleRoot(const std::string& value, ServerConfig& config);
        // static void handleIndex(const std::string& value, ServerConfig& config);
        // static void handleClientMaxBodySize(const std::string& value, ServerConfig& config);
        // static void handleErrorPage(const std::string& value, ServerConfig& config);

    public:
        
        ConfParser(void);
        ~ConfParser(void);

        static std::vector<ServerConfig> parseFile(const char* fileName);
};
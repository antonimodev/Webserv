//#include "Webserv.hpp"
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "../../Exceptions/ParseException/ParseException.hpp"
#include "ConfParser.hpp"
#include "../Parser/Parser.hpp"
#include "webserv.h"

ServerConfig::ServerConfig(void)
    : server_name(""),
      listen_port(0),
      host(""),
      root(""),
      index(""),
      client_max_body_size(0),
      error_pages(),
      locations()
{}

LocationConfig::LocationConfig(void)
	: allowed_methods(),
	  root(""),
	  upload_path(""),
	  autoindex(false),
	  cgi_extension(std::make_pair("", "")),
	  redirect(std::make_pair(0, ""))
{}

void	ConfParser::initHandlers(void) {
	_serverHandlers["server_name"] = &handleServerName;
	_serverHandlers["listen"] = &handleListen;
    _serverHandlers["host"] = &handleHost;
    _serverHandlers["root"] = &handleRoot;
    _serverHandlers["index"] = &handleIndex;
    _serverHandlers["client_max_body_size"] = &handleClientMaxBodySize;
    _serverHandlers["error_page"] = &handleErrorPage;

	_locationHandlers["allowed_methods"] = &handleAllowedMethods;
    _locationHandlers["root"] = &handleLocationRoot;
    _locationHandlers["upload_path"] = &handleUploadPath;
    _locationHandlers["autoindex"] = &handleAutoindex;
    _locationHandlers["cgi_extension"] = &handleCgiExtension;
    _locationHandlers["return"] = &handleReturn;
}

void ConfParser::handleServerName(const std::string& value, ServerConfig& config) {
	(void)value;
	(void)config;
}

void ConfParser::handleListen(const std::string& value, ServerConfig& config) {
	(void)value;
	(void)config;
}

void ConfParser::handleHost(const std::string& value, ServerConfig& config) {
	(void)value;
	(void)config;
}

void ConfParser::handleRoot(const std::string& value, ServerConfig& config) {
	(void)value;
	(void)config;
}

void ConfParser::handleIndex(const std::string& value, ServerConfig& config) {
	(void)value;
	(void)config;
}

void ConfParser::handleClientMaxBodySize(const std::string& value, ServerConfig& config) {
	(void)value;
	(void)config;
}

void ConfParser::handleErrorPage(const std::string& value, ServerConfig& config) {
	(void)value;
	(void)config;
}

void ConfParser::handleAllowedMethods(const std::string& value, LocationConfig& location) {
	(void)value;
	(void)location;
}

void ConfParser::handleLocationRoot(const std::string& value, LocationConfig& location) {
	(void)value;
	(void)location;
}

void ConfParser::handleUploadPath(const std::string& value, LocationConfig& location) {
	(void)value;
	(void)location;
}

void ConfParser::handleAutoindex(const std::string& value, LocationConfig& location) {
	(void)value;
	(void)location;
}

void ConfParser::handleCgiExtension(const std::string& value, LocationConfig& location) {
	(void)value;
	(void)location;
}

void ConfParser::handleReturn(const std::string& value, LocationConfig& location) {
	(void)value;
	(void)location;
}


ConfParser::ConfParser(const char* fileName) {
	initHandlers();
	parseFile(fileName);
}

ConfParser::~ConfParser(void) {}

static std::vector<std::string> tokenizeContent(const std::string& buffer) {
    std::stringstream ss(buffer);
    std::vector<std::string> tokenizedBuffer;
    std::string line;

    while (std::getline(ss, line)) {
        // Buscar el inicio de un comentario
        size_t commentPos = line.find('#');
        
        // Si hay comentario, eliminar desde '#' hasta el final
        if (commentPos != std::string::npos)
            line = line.substr(0, commentPos);
        
        // Tokenizar la línea (sin comentario)
        std::stringstream lineStream(line);
        std::string token;
        
        while (lineStream >> token)
            tokenizedBuffer.push_back(token);
    }

    return tokenizedBuffer;
}

std::string ConfParser::getValue(const std::vector<std::string>& content, size_t& i) {
    std::string value;
    
    ++i;  // Avanzar al primer valor
    
    while (i < content.size()) {
        const std::string& token = content[i];
        
        if (!token.empty() && token[token.length() - 1] == ';') {
            if (token.length() > 1) {
                if (!value.empty())
                    value += " ";
                value += token.substr(0, token.length() - 1);
            }
            break;
        }

        // Acumular valores multi-token (ej: "GET POST DELETE")
        if (!value.empty())
            value += " ";
        value += token;
        
        ++i;
    }
    
    return value;
}


void ConfParser::parseToken(const std::string& token, const std::vector<std::string>& content, size_t& i) {
	(void)token;
	
	if (token == "server" && _brackets.empty()) {
		_servers.push_back(ServerConfig());
		return;
	}

	if (token == "location" && !_brackets.empty()) {
		if (i + 1 >= content.size())
            throw ParseException("Error: 'location' without path");
		
		std::string path = content[i + 1];
		_servers.back().locations[path] = LocationConfig();
		++i;
		return;
	}
	
	if (token == "{") {
		_brackets.push(token);
		return;
	}
	
	if (token == "}") {
		if (_brackets.empty())
			throw ParseException("Unmatched closing bracket");
		_brackets.pop();
		return;
	}


	if (!_brackets.empty()) {
		bool isServerContext = (_brackets.size() == 1);
		bool isLocationContext = (_brackets.size() == 2);
		
		if (!isServerContext && !isLocationContext)
			throw ParseException("Invalid nesting level for token: " + token);
		
		// Verificar que hay un valor siguiente
		if (i + 1 >= content.size())
			throw ParseException("Missing value for directive: " + token);
		
		// Extraer valor hasta ';'
		std::string value = getValue(content, i);
		
		// Ejecutar handler según contexto
		if (isServerContext) {
			std::map<std::string, ServerHandler>::iterator it = 
				_serverHandlers.find(token);
			
			if (it == _serverHandlers.end())
				throw ParseException("Unknown server directive: " + token);
			
			it->second(value, _servers.back());
			return;
		}
		
		if (isLocationContext) {
			std::map<std::string, LocationHandler>::iterator it = 
				_locationHandlers.find(token);
			
			if (it == _locationHandlers.end())
				throw ParseException("Unknown location directive: " + token);
			
			// Obtener la última location añadida
			std::map<std::string, LocationConfig>::reverse_iterator lastLoc = 
				_servers.back().locations.rbegin();
			
			if (lastLoc == _servers.back().locations.rend())
				throw ParseException("No location context available");
			
			it->second(value, lastLoc->second);
			return;
		}
	}

	throw ParseException("Unexpected token: " + token);
}

void ConfParser::parseFile(const char* fileName) {
    std::string buffer;
	std::vector<std::string> content;

	buffer = get_file_content(fileName);
	content = tokenizeContent(buffer);

	_servers.clear();

    for (size_t i = 0; i < content.size(); ++i) {
		std::string token = content[i];


		parseToken(token, content, i);
	}

	if (!_brackets.empty())
		throw ParseException("Unmatched opening bracket");

	std::cout << "Printing _servers:\n" << std::endl;
	for (size_t s = 0; s < _servers.size(); ++s) {
		std::cout << "Server " << s << ":" << std::endl;
		std::cout << "  server_name: " << _servers[s].server_name << std::endl;
		std::cout << "  listen_port: " << _servers[s].listen_port << std::endl;
		std::cout << "  host: " << _servers[s].host << std::endl;
		std::cout << "  root: " << _servers[s].root << std::endl;
		std::cout << "  index: " << _servers[s].index << std::endl;
		std::cout << "  client_max_body_size: " << _servers[s].client_max_body_size << std::endl;
		std::cout << "  error_pages:" << std::endl;
		for (std::map<int, std::string>::iterator it = _servers[s].error_pages.begin(); it != _servers[s].error_pages.end(); ++it) {
			std::cout << "    " << it->first << " -> " << it->second << std::endl;
		}
		std::cout << "  locations:" << std::endl;
		for (std::map<std::string, LocationConfig>::iterator it = _servers[s].locations.begin(); it != _servers[s].locations.end(); ++it) {
			std::cout << "    Location " << it->first << ":" << std::endl;
			std::cout << "      allowed_methods: ";
			for (size_t m = 0; m < it->second.allowed_methods.size(); ++m) {
				std::cout << it->second.allowed_methods[m] << " ";
			}
			std::cout << std::endl;
			std::cout << "      root: " << it->second.root << std::endl;
			std::cout << "      upload_path: " << it->second.upload_path << std::endl;
			std::cout << "      autoindex: " << (it->second.autoindex ? "true" : "false") << std::endl;
			std::cout << "	  cgi_extension: " << it->second.cgi_extension.first << " -> " << it->second.cgi_extension.second << std::endl;
			std::cout << "      redirect: " << it->second.redirect.first << " -> " << it->second.redirect.second << std::endl;
		}
		std::cout << std::endl;
	}



	// cuando terminemos, _servers ha de tener todo parseado.
}
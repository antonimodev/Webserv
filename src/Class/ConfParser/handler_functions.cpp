#include <sstream>
#include <set>

#include "ConfParser.hpp"
#include "ParseException.hpp"


struct AllowedMethods {
	static bool isValid(const std::string& method) {
		return	method == "GET" ||
				method == "POST" ||
				method == "DELETE" ||
				method == "PUT";
	}
};


void	handleServerName(const std::string& value, ServerConfig& config) {
    config.server_name = value;
}


void	handleListen(const std::string& value, ServerConfig& config) {
    std::istringstream iss(value);
    if (!(iss >> config.listen_port))
        throw ParseException("Invalid port number: " + value);
}


void	handleHost(const std::string& value, ServerConfig& config) {
    config.host = value;
}


void	handleRoot(const std::string& value, ServerConfig& config) {
    config.root = value;
}


void	handleIndex(const std::string& value, ServerConfig& config) {
    config.index = value;
}


void	handleClientMaxBodySize(const std::string& value, ServerConfig& config) {
    std::istringstream iss(value);
    std::string sizeStr;
    iss >> sizeStr;

    size_t multiplier = 1;

    if (!sizeStr.empty()) {
        char suffix = sizeStr[sizeStr.length() - 1];

        if (suffix == 'M' || suffix == 'm')
            multiplier = 1024 * 1024;
        else if (suffix == 'K' || suffix == 'k')
            multiplier = 1024;

        sizeStr = sizeStr.substr(0, sizeStr.length() - 1);
    }

    size_t num;
    std::istringstream numStream(sizeStr);
    if (!(numStream >> num))
        throw ParseException("Invalid client_max_body_size: " + value);

    config.client_max_body_size = num * multiplier;
}


void	handleErrorPage(const std::string& value, ServerConfig& config) {
    std::istringstream iss(value);
    int errorCode;
    std::string path;
    
    if (!(iss >> errorCode >> path))
        throw ParseException("Invalid error_page format: " + value);
    
    config.error_pages[errorCode] = path;
}


void	handleAllowedMethods(const std::string& value, LocationConfig& location) {
    std::istringstream iss(value);
    std::string method;

    location.allowed_methods.clear();

    while (iss >> method) {
        if (!AllowedMethods::isValid(method))
            throw ParseException("Invalid HTTP method: " + method);

        location.allowed_methods.push_back(method);
    }

    if (location.allowed_methods.empty())
        throw ParseException("allowed_methods cannot be empty");
}


void	handleLocationRoot(const std::string& value, LocationConfig& location) {
    location.root = value;
}


void	handleUploadPath(const std::string& value, LocationConfig& location) {
    location.upload_path = value;
}


void	handleAutoindex(const std::string& value, LocationConfig& location) {
    if (value == "on" || value == "true" || value == "1")
        location.autoindex = true;
    else if (value == "off" || value == "false" || value == "0")
        location.autoindex = false;
    else
        throw ParseException("Invalid autoindex value: " + value);
}


void	handleCgiExtension(const std::string& value, LocationConfig& location) {
    std::istringstream iss(value);
    std::string extension, path;
    
    if (!(iss >> extension >> path))
        throw ParseException("Invalid cgi_extension format: " + value);
    
    location.cgi_extension = std::make_pair(extension, path);
}


void	handleReturn(const std::string& value, LocationConfig& location) {
    std::istringstream iss(value);
    int code;
    std::string url;
    
    if (!(iss >> code >> url))
        throw ParseException("Invalid return format: " + value);
    
    location.redirect = std::make_pair(code, url);
}
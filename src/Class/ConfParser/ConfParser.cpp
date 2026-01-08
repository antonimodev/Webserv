#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "ConfParser.hpp"
#include "Parser.hpp"
#include "webserv.h"

#include "ParseException.hpp"


ServerConfig::ServerConfig(void)
	:	server_names(),
		listen_port(0),
		host(""),
		root(""),
		index(""),
		client_max_body_size(0),
		error_pages(),
		locations()
{}


LocationConfig::LocationConfig(void)
	:	path(""),
		allowed_methods(),
		index(""),
		root(""),
		upload_path(""),
		autoindex(false),
		cgi_extension(std::make_pair("", "")),
		redirect(std::make_pair(0, ""))
{}


LocationConfig::LocationConfig(std::string path)
	:	path(path),
		allowed_methods(),
		index(""),
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
	_locationHandlers["index"] = &handleLocationIndex;
	_locationHandlers["upload_path"] = &handleUploadPath;
	_locationHandlers["autoindex"] = &handleAutoindex;
	_locationHandlers["cgi_extension"] = &handleCgiExtension;
	_locationHandlers["return"] = &handleReturn;
}


// CONSTRUCTORS

ConfParser::ConfParser(void) {
	initHandlers();
}


ConfParser::ConfParser(const char* fileName) {
	initHandlers();
	parseFile(fileName);
}


ConfParser::ConfParser(const ConfParser& other)
	:	_brackets(other._brackets),
		_servers(other._servers),
		_current_location_path(other._current_location_path)
		{
	initHandlers();
}


ConfParser& ConfParser::operator=(const ConfParser& other) {
	if (this != &other) {
		_brackets = other._brackets;
		_servers = other._servers;
		_current_location_path = other._current_location_path;
		initHandlers();
	}
	return *this;
}


ConfParser::~ConfParser(void) {}


// FUNCTIONS

std::vector<std::string> ConfParser::tokenizeContent(const std::string& buffer) {
	std::stringstream ss(buffer);
	std::vector<std::string> tokenizedBuffer;
	std::string line;

	// Tokenize line by line, ignoring comments
	while (std::getline(ss, line)) {
		size_t commentPos = line.find('#');
		
		if (commentPos != std::string::npos)
			line = line.substr(0, commentPos);

		std::stringstream lineStream(line);
		std::string token;

		while (lineStream >> token)
			tokenizedBuffer.push_back(token);
	}
	return tokenizedBuffer;
}


static std::string getValue(const std::vector<std::string>& content, size_t& i) {
	std::string value;

	++i;
	if (i >= content.size())
		throw ParseException("Error: unexpected end of file after directive");

	while (i < content.size()) {
		const std::string& token = content[i];
		
		if (!token.empty() && token[token.length() - 1] == ';') {
			if (token.length() > 1) {
				if (!value.empty())
					value += " ";
				value += token.substr(0, token.length() - 1);
			}
			return value;
		}

		if (!value.empty())
			value += " ";
		value += token;
		
		++i;
	}

	throw ParseException("Error: missing semicolon after directive value");
}


void	ConfParser::saveToken(std::string token, const std::vector<std::string>& content, size_t& i) {
	bool isServerContext = (_brackets.size() == 1);
	bool isLocationContext = (_brackets.size() == 2);

	if (!isServerContext && !isLocationContext)
		throw ParseException("Error: invalid nesting level for token: " + token);

	if (i + 1 >= content.size())
		throw ParseException("Error: missing value for directive: " + token);

	std::string value = getValue(content, i);

	if (isServerContext) {
		std::map<std::string, ServerHandler>::iterator it = _serverHandlers.find(token);

		if (it == _serverHandlers.end())
			throw ParseException("Error: unknown server directive: " + token);

		it->second(value, _servers.back());
		return;
	}

	if (isLocationContext) {
		std::map<std::string, LocationHandler>::iterator it = _locationHandlers.find(token);

		if (it == _locationHandlers.end())
			throw ParseException("Error: unknown location directive: " + token);

		if (_current_location_path.empty())
			throw ParseException("Error: no location context available");

		LocationConfig& currentLocation = _servers.back().locations[_current_location_path];
		it->second(value, currentLocation);
		return;
	}
}


void	ConfParser::parseToken(const std::string& token, const std::vector<std::string>& content, size_t& i) {
	if (token == "server" && _brackets.empty()) {
		_servers.push_back(ServerConfig());
		return;
	}

	if (token == "location" && !_brackets.empty()) {
		if (i + 1 >= content.size())
			throw ParseException("Error: 'location' without path");

		std::string path = content[i + 1];

		if (_servers.back().locations.find(path) != _servers.back().locations.end())
			throw ParseException("Error: duplicate location block: " + path);
		
		_servers.back().locations[path] = LocationConfig(path);

		_current_location_path = path;
		++i;
		return;
	}

	if (token == "{") {
		_brackets.push(token);
		return;
	}

	if (token == "}") {
		if (_brackets.empty())
			throw ParseException("Error: unmatched closing bracket");
		
		_brackets.pop();

		if (_brackets.size() == 1)
			_current_location_path.clear();

		return;
	}

	if (_brackets.empty())
		throw ParseException("Error: unexpected token: " + token);

	saveToken(token, content, i);
}


void	ConfParser::validateServers(void) {
	if (_servers.empty())
		throw ParseException("Error: configuration must have at least one server block");

	for (size_t i = 0; i < _servers.size(); ++i) {
		if (_servers[i].listen_port == 0)
			throw ParseException("Error: server must have 'listen' directive");

		if (_servers[i].server_names.empty())
			_servers[i].server_names.push_back("");

		if (_servers[i].root.empty())
			_servers[i].root = "./static";

		if (_servers[i].index.empty())
			_servers[i].index = "index.html";

		if (_servers[i].host.empty())
			throw ParseException("Error: server must have 'host' directive");

		if (_servers[i].host.find_first_not_of("0123456789.") != std::string::npos)
			throw ParseException("Error: invalid host format: " + _servers[i].host);

		if (_servers[i].listen_port < 1 || _servers[i].listen_port > 65535)
			throw ParseException("Error: invalid port number (must be 1-65535)");

		std::map<std::string, LocationConfig>::iterator it = _servers[i].locations.begin();

		for (; it != _servers[i].locations.end(); ++it) {
			if (it->second.allowed_methods.empty())
				throw ParseException("Location '" + it->first + "' must have at least one allowed method");
		}
	}
}


void	ConfParser::parseFile(const char* fileName) {
	std::string buffer;
	std::vector<std::string> content;

	buffer = get_file_content(fileName);
	content = tokenizeContent(buffer);

	_servers.clear();

	for (size_t i = 0; i < content.size(); ++i)
		parseToken(content[i], content, i);

	if (!_brackets.empty())
		throw ParseException("Error: unmatched opening bracket");

	validateServers();
}


const std::vector<ServerConfig>& ConfParser::getServers() const {
	return _servers;
}

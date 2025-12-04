//#include "Webserv.hpp"
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "../../Exceptions/ParseException.hpp"
#include "ConfParser.hpp"



std::stringstream ConfParser::readContent(const char* fileName) {
	std::fstream file(fileName);

	if (file.is_open() == 0)
		throw OpenFailed() ;

	std::stringstream buffer;
	buffer << file.rdbuf();

	return buffer;
}



std::vector<ServerConfig> ConfParser::parseFile(const char* fileName) {
    std::stringstream buffer = readContent(fileName);
    std::vector<ServerConfig> serverConfigs;
	
    std::string token;
	std::vector<std::string> tokens;
    while (buffer >> token)
		tokens.push_back(token);

    return serverConfigs;
}

int main(void) {

	try {
		ConfParser::parseFile("src/Class/ConfParser/file.conf");
	} catch ( const OpenFailed& e ) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
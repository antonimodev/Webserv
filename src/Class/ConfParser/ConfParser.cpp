//#include "Webserv.hpp"
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>

#include "../../Exceptions/ParseException.hpp"

std::stringstream readContent(const char* fileName) {
	std::fstream file(fileName);

	if (file.is_open() == 0)
		throw OpenFailed() ;

	std::stringstream buffer;
	buffer << file.rdbuf();

	return buffer;
}

void	parseFile(const char* fileName) {
	std::stringstream buffer;

	buffer = readContent(fileName);
}

int main(void) {

	try {
		parseFile("file.conf");
	} catch ( const OpenFailed& e ) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
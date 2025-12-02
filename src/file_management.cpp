#include <fstream>
#include <sstream>
#include <iostream>

#include "HttpCodeException.hpp"


std::string	get_file_content(const std::string& path) {
	std::ifstream	file(path.c_str(), std::ios::binary);

	if (!file.is_open())
		throw HttpCodeException(NOT_FOUND, "Error: cannot open file " + path);

	std::ostringstream	msg;

	msg << file.rdbuf();

	std::string content = msg.str();

	return content;
}
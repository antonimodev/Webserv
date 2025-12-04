#include <fstream>
#include <sstream>
#include <iostream>

#include <dirent.h>

#include "HttpCodeException.hpp"
#include "DirectoryHandle.hpp"

namespace webserv {

    std::string	get_directory_list(const std::string& path, const std::string& route) {
        webserv::DirectoryHandle dir(path);

        std::string html_body = "<html><body><h1>Index of " + route + "</h1>\n";
        struct dirent* directoryItem;

        while ((directoryItem = readdir(dir.getDirectory())) != NULL) {
            std::string fileName = directoryItem->d_name;

            if (fileName == "." || fileName == "..") 
                continue;

            html_body += "<a href=\"" + fileName + "\">" + fileName + "</a>\n";
        }

        html_body += "</body></html>";
        return html_body;
    }
}

std::string	get_file_content(const std::string& path) {
	std::ifstream	file(path.c_str(), std::ios::binary);

	if (!file.is_open())
		throw HttpCodeException(NOT_FOUND, "Error: cannot open file " + path);

	std::ostringstream	msg;

	msg << file.rdbuf();

	std::string content = msg.str();

	return content;
}

std::string	get_extension(const std::string& route) {
	std::string::size_type dot_pos = route.rfind('.');
	std::string::size_type slash_pos = route.rfind('/');

	// No dot found, or dot is before the last slash (e.g., "/folder.d/file")
	if (dot_pos == std::string::npos)
		return "";
	if (slash_pos != std::string::npos && dot_pos < slash_pos)
		return "";

	return route.substr(dot_pos + 1);
}
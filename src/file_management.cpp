#include <fstream>
#include <sstream>
#include <iostream>

#include <dirent.h>

#include <cstdio>
#include <sys/stat.h>

#include "HttpCodeException.hpp"
#include "DirectoryHandle.hpp"

namespace webserv {

    std::string	get_directory_list(const std::string& path, const std::string& route) {
        webserv::DirectoryHandle dir(path);

        std::string html_body = "<body><h1>Index of " + route + "</h1>\n";
        struct dirent* directoryItem;

        while ((directoryItem = readdir(dir.getDirectory())) != NULL) {
            std::string fileName = directoryItem->d_name;

            if (fileName == "." || fileName == "..") 
                continue;

            html_body += "<a href=\"" + fileName + "\">" + fileName + "</a>\n";
        }

        html_body.append("</body>");
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

	// No dot found, or dot is before the last slash (e.g., "/folder.d/file.html")
	if (dot_pos == std::string::npos)
		return "";
	if (slash_pos != std::string::npos && dot_pos < slash_pos)
		return "";

	return route.substr(dot_pos + 1);
}


std::string	delete_resource(const std::string& path) {
	struct stat info;
	
	if (stat(path.c_str(), &info) == -1)
		throw HttpCodeException(NOT_FOUND, "Error: file " + path + " not found");
	
	if (S_ISDIR(info.st_mode))
		throw HttpCodeException(FORBIDDEN, "Error: cannot delete directory");

	if (std::remove(path.c_str()) != 0)
		throw HttpCodeException(INTERNAL_ERROR, "Error: ");

	return "HTTP/1.1 204 No Content\r\n"
			"Content-Length: 0 \r\n"
			"\r\n";
}
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <vector>
#include <dirent.h>
#include <cstdio>
#include <sys/stat.h>
#include <cerrno>

#include "DirectoryHandle.hpp"
#include "webserv.h"

#include "HttpCodeException.hpp"


std::string	get_directory_list(const std::string& path, const std::string& route) {
	DirectoryHandle dir(path);

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


std::string	get_file_content(const std::string& path) {
	std::ifstream	file(path.c_str(), std::ios::binary);

	if (!file.is_open())
		throw HttpCodeException(NOT_FOUND, "Error: cannot open file " + path);

	std::ostringstream	msg;
	msg << file.rdbuf();

	std::string content = msg.str();

	return content;
}


std::string	get_fd_content(int fd) {
	char		buffer[4096];
	ssize_t		bytes_read;
	std::string	content;
	
	while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0)
		content.append(buffer, bytes_read);

	if (bytes_read == -1)
		throw HttpCodeException(INTERNAL_ERROR, "Error: failed to read from fd");

	return content;
}


ssize_t	readFd(int fd, std::string& buffer, FdType type) {
	char temp[4096];
	ssize_t bytes_read = -1;

	if (type == SOCKET)
		bytes_read = recv(fd, temp, sizeof(temp) - 1, 0);
	else
		bytes_read = read(fd, temp, sizeof(temp) - 1);

	if (bytes_read > 0) {
		temp[bytes_read] = '\0';
		buffer.append(temp);
	}

	return bytes_read;
}


std::string	get_extension(const std::string& route) {
	std::string::size_type dot_pos = route.rfind('.');
	std::string::size_type slash_pos = route.rfind('/');

	if (dot_pos == std::string::npos)
		return "";
	if (slash_pos != std::string::npos && dot_pos < slash_pos)
		return "";

	return route.substr(dot_pos);
}


/*****************************************************************************
*                           RESOURCE MANAGEMENT                              *
*****************************************************************************/

std::string	load_resource(const std::string& full_path, const std::string& route, bool autoindex, const std::string& index_file) {
	struct stat info;

	std::string body;
	std::string content_type;

	std::cout << "\nfull path: " << full_path << std::endl;

	if (stat(full_path.c_str(), &info) != 0)
		throw HttpCodeException(NOT_FOUND, "Error: file not found");

	if (S_ISDIR(info.st_mode)) {
		std::string index_path = full_path;

		if (!index_path.empty() && index_path[index_path.size() - 1] != '/')
			index_path += "/";

		index_path += index_file;

		if (stat(index_path.c_str(), &info) == 0) {
			body = get_file_content(index_path);
			content_type = "text/html";
		} else if (autoindex) {
			body = get_directory_list(full_path, route);
			content_type = "text/html";
		} else
			throw HttpCodeException(FORBIDDEN, "Error: directory listing denied");
	} else {
		body = get_file_content(full_path);
		content_type = get_mime_type(get_extension(full_path));
	}

	std::ostringstream oss;
	oss << body.size();

	return "HTTP/1.1 200 OK\r\n"
		"Content-Type: " + content_type + "\r\n"
		"Content-Length: " + oss.str() + "\r\n"
		"\r\n" + body;
}


std::string	delete_resource(const std::string& path) {
	struct stat info;

	if (stat(path.c_str(), &info) == -1)
		throw HttpCodeException(NOT_FOUND, "Error: file " + path + " not found");

	if (S_ISDIR(info.st_mode))
		throw HttpCodeException(FORBIDDEN, "Error: cannot delete directory");

	if (access(path.c_str(), W_OK) != 0)
		throw HttpCodeException(FORBIDDEN, "Error: no write permission for " + path);

	if (std::remove(path.c_str()) != 0)
		throw HttpCodeException(INTERNAL_ERROR, "Error: failed to delete file " + path);

	return "HTTP/1.1 204 No Content\r\n"
			"Content-Length: 0 \r\n"
			"\r\n";
}


std::string	save_resource(const std::string& full_path, const std::string& body) {
	std::ofstream	file(full_path.c_str(), std::ios::binary);

	if (!file.is_open())
		throw HttpCodeException(INTERNAL_ERROR, "Error: cannot open or write file to POST");

	file.write(body.data(), body.size());

	if (file.fail())
		throw HttpCodeException(INTERNAL_ERROR, "Error: failed to write file content");

	return "HTTP/1.1 201 Created\r\n"
		"Content-Length: 0 \r\n"
		"\r\n";
}


/*****************************************************************************
*                             UTILITIES                                      *
*****************************************************************************/

std::string trim_space(const std::string& text) {
	size_t start = text.find_first_not_of(' ');
	size_t end = text.find_last_not_of(' ');

	return text.substr(start, end - start + 1);
}

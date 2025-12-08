#pragma once

#include <iostream>
#include <vector>

// --- FILE MANAGEMENT ---

namespace webserv {
    std::string	get_directory_list(const std::string& path, const std::string& route);
}

std::string	get_file_content(const std::string& path);
std::string	get_extension(const std::string& route);
const std::string   get_mime_type(const std::string& extension);


std::string	delete_resource(const std::string& path);
std::string	load_resource(const std::string& full_path, const std::string& route, std::string& content_type);



// Testing
std::vector<std::string> split(const std::string& text, const std::string& delimiter);
std::string trim_space(const std::string& text);
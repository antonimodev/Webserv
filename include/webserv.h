#pragma once

// --- FILE MANAGEMENT ---

namespace webserv {
    std::string	get_directory_list(const std::string& path, const std::string& route);
}

std::string	get_file_content(const std::string& path);
std::string	get_extension(const std::string& route);
std::string	delete_resource(const std::string& path);


// --- PARSE ---
const std::string   get_mime_type(const std::string& extension);
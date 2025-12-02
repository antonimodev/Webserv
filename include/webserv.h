#pragma once

// --- FILE MANAGEMENT ---
std::string	get_file_content(const std::string& path);

// --- PARSE ---
const std::string get_mime_type(const std::string& extension);
#pragma once

#include <string>
#include <vector>

#include <sys/types.h>


// --- FILE MANAGEMENT ---

namespace webserv {
	/**
	 * @brief Generates HTML directory listing.
	 * @param path Filesystem path to directory.
	 * @param route HTTP route for href generation.
	 * @return HTML string with directory contents.
	 */
	std::string get_directory_list(const std::string& path, const std::string& route);
}

enum FdType {SOCKET, PIPE};

/**
 * @brief Reads entire file contents into string.
 * @param path Path to file.
 * @return File contents as string.
 * @throws HttpCodeException if file cannot be opened.
 */
std::string get_file_content(const std::string& path);

/**
 * @brief Reads entire fd content into string.
 * @param fd file descriptor to read.
 * @return Fd contents as string.
 * @throws HttpCodeException if file cannot be opened.
 */
std::string	get_fd_content(int fd);

ssize_t	readFd(int fd, std::string& buffer, FdType type);

/**
 * @brief Extracts file extension from path.
 * @param route Path or route string.
 * @return Extension without dot, or empty string.
 */
std::string get_extension(const std::string& route);

/**
 * @brief Gets MIME type for file extension.
 * @param extension File extension without dot.
 * @return MIME type string.
 */
const std::string get_mime_type(const std::string& extension);


// --- RESOURCE OPERATIONS ---

/**
 * @brief Saves content to file (POST handler).
 * @param full_path Filesystem path.
 * @param body Content to write.
 * @return HTTP 201 response.
 * @throws HttpCodeException on error.
 */
std::string save_resource(const std::string& full_path, const std::string& body);

/**
 * @brief Loads file or directory listing (GET handler).
 * @param full_path Filesystem path.
 * @param route HTTP route for directory listing.
 * @return HTTP 200 response with content.
 * @throws HttpCodeException on error.
 */
std::string load_resource(const std::string& full_path, const std::string& route,  const std::string& index_file);

/**
 * @brief Deletes a file (DELETE handler).
 * @param path Filesystem path.
 * @return HTTP 204 response.
 * @throws HttpCodeException on error.
 */
std::string delete_resource(const std::string& path);


// --- UTILITIES ---

/**
 * @brief Splits string by delimiter.
 * @param text Input string.
 * @param delimiter Delimiter string.
 * @return Vector of substrings.
 * @throws std::runtime_error if delimiter not found.
 */
std::vector<std::string> split(const std::string& text, const std::string& delimiter);

/**
 * @brief Trims leading/trailing spaces.
 * @param text Input string.
 * @return Trimmed string.
 */
std::string trim_space(const std::string& text);
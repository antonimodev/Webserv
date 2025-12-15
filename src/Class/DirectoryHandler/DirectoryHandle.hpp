#pragma once

#include <dirent.h>
#include <string>
#include <stdexcept>

namespace webserv {

/**
 * @brief RAII wrapper for DIR* to ensure proper resource cleanup.
 * 
 * Prevents resource leaks and double free by disabling copy operations.
 */
class DirectoryHandle {
	private:
		DIR*	_directory;

		// Private because it wont be used, and don't need named params in this case

		DirectoryHandle(const DirectoryHandle&);
		DirectoryHandle& operator=(const DirectoryHandle&);

	public:
		/**
		 * @brief Constructor. Opens the directory.
		 * @param path Path to the directory.
		 * @throw std::runtime_error if the directory cannot be opened.
		 */
		explicit DirectoryHandle(const std::string& path);

		/**
		 * @brief Destructor. Automatically closes the directory.
		 */
		~DirectoryHandle(void);

		/**
		 * @brief Gets the raw DIR* pointer for use with readdir.
		 * @return DIR* pointer managed by this guard.
		 */
		DIR* getDirectory(void) const;
	};
}
#include "DirectoryHandle.hpp"
#include "HttpCodeException.hpp" 

namespace webserv {

	DirectoryHandle::DirectoryHandle(const std::string& path) {
		_directory = opendir(path.c_str());
		if (!_directory)
			throw HttpCodeException(NOT_FOUND, "DirectoryHandle: Failed to open " + path);
	}

	DirectoryHandle::~DirectoryHandle(void) {
		if (_directory)
			closedir(_directory);
	}

	DIR* DirectoryHandle::getDirectory(void) const {
		return _directory;
	}

}
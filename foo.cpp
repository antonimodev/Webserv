#include <sys/stat.h>
#include <dirent.h>
#include <iostream>

#include "HttpCodeException.hpp"

void foo(std::string& path) {
	struct stat info;

	if (stat(path.c_str(), &info) == -1) {
		throw HttpCodeException(NOT_FOUND, "Error: file " + path + " not found");
	} else {
		if (S_ISDIR(info.st_mode)) {
			std::cout << "Directory" << std::endl;
			std::string indexPath = path.append("/index.html");

			if (stat(indexPath.c_str(), &info) == -1) {
				if (autoindex) {
					DIR* directory = opendir(path.c_str());

					if (!directory)
						throw HttpCodeException (NOT_FOUND, "Error: cannot open directory " + path);

					struct dirent* directoryItem;
					while ((directoryItem = readdir(directory)) != NULL)
						std::cout << directoryItem->d_name << std::endl;

					closedir(directory);
				} else
					throw HttpCodeException(FORBIDDEN, "Error: directory listing forbbiden for " + path);
			}
		} else if (S_ISREG(info.st_mode))
			std::cout << "Regular file" << std::endl; // default
	}
}
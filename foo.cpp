#include <sys/stat.h>
#include <dirent.h>
#include <iostream>

//#include "HttpCodeException.hpp"

void foo(const std::string& path) {
	struct stat info;

	if (stat(path.c_str(), &info) == -1) {
		std::cerr << "Error: file " + path + " not found" << std::endl;
		//throw HttpCodeException(NOT_FOUND, "Error: file " + path + " not found");
	} else {
		if (S_ISDIR(info.st_mode)) {
			std::cout << "Directory" << std::endl;
			std::string indexPath = path + "/index.html"; // avoid append() to not modify original

			if (stat(indexPath.c_str(), &info) == -1) {
				if (true) {
					DIR* directory = opendir(path.c_str());

					if (!directory)
						std::cerr << "Error: cannot open " + path << std::endl;
						//throw HttpCodeException (NOT_FOUND, "Error: cannot open directory " + path);

					struct dirent* directoryItem;
					while ((directoryItem = readdir(directory)) != NULL)
						std::cout << directoryItem->d_name << std::endl;

					closedir(directory);
				} else
					std::cerr << "Error: directory listing forbbiden for " + path << std::endl;
					//throw HttpCodeException(FORBIDDEN, "Error: directory listing forbbiden for " + path);
			}
		} else if (S_ISREG(info.st_mode))
			std::cout << "Regular file" << std::endl; // default
	}
}

int	main(void) {
	std::string main = "main.cpp";
	std::string dir = "include/";
	foo(main);
	foo(dir);
}

// "<a href=\"" + directoryItem->d_name + "\">" + directoryItem->d_name + "</a>";
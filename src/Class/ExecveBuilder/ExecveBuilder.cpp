#include <map>
#include <vector>
#include <string>
#include <cstring>

#include "ExecveBuilder.hpp"

// CONSTRUCTORS

ExecveBuilder::ExecveBuilder(const std::map<std::string, std::string> env_map) {
	_matrix = new char*[env_map.size() + 1];

	size_t i = 0;
	std::map<std::string, std::string>::const_iterator it;
	
	for (it = env_map.begin(); it != env_map.end(); ++it) {
		std::string tmp = it->first + "=" + it->second;
		_matrix[i] = new char[tmp.size() + 1];
		std::strcpy(_matrix[i], tmp.c_str());
		++i;
	}
	_matrix[i] = NULL;
}


ExecveBuilder::ExecveBuilder(const std::vector<std::string> argv) {
	_matrix = new char*[argv.size() + 1];

	for (size_t i = 0; i < argv.size(); ++i) {
		_matrix[i] = new char[argv[i].size() + 1];
		std::strcpy(_matrix[i], argv[i].c_str());
	}

	_matrix[argv.size()] = NULL;
}

ExecveBuilder::~ExecveBuilder(void) {
	if (_matrix) {
		for (size_t i = 0; _matrix[i]; ++i)
			delete[] _matrix[i];
		delete[] _matrix;
	}
}


// PUBLIC METHODS


char** ExecveBuilder::get(void) const {
	return _matrix;
}
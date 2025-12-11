#pragma once

#include <map>

class ExecveBuilder {
	private:
		char**	_matrix;

		ExecveBuilder(const ExecveBuilder&);
		ExecveBuilder& operator=(const ExecveBuilder&);

	public:
		ExecveBuilder(const std::map<std::string, std::string> env_map);
		ExecveBuilder(const std::vector<std::string> argv);
		~ExecveBuilder(void);

		char**	get(void) const;
};
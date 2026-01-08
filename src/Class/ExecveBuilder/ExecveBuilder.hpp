#pragma once

#include <map>


class ExecveBuilder {
	private:
		char**	_matrix;

		ExecveBuilder(const ExecveBuilder&);
		ExecveBuilder& operator=(const ExecveBuilder&);

	public:
		/**
		 * @brief Construct a new Execve Builder object. Sets up the char** matrix for execve.
		 * 
		 * @param std::map<std::string, std::string> env_map Environment variables as a map 
		 */
		ExecveBuilder(const std::map<std::string, std::string> env_map);

		/**
		 * @brief Construct a new Execve Builder object. Sets up the char** matrix for execve.
		 * 
		 * @param std::vector<std::string> argv Argument vector as a vector of strings
		 */
		ExecveBuilder(const std::vector<std::string> argv);
		~ExecveBuilder(void);

		/**
		 * @brief Get the char** matrix for execve.
		 * 
		 * @return char** The char** matrix
		 */
		char**	get(void) const;
};

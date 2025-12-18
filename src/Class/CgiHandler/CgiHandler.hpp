#pragma once

#include <map>
#include <string>

#include "Parser.hpp"

// CGI -> Common Gateway Interface

class CgiHandler {
    private:
        std::map<std::string, std::string>  _env;

        CgiHandler(const CgiHandler&);
        CgiHandler& operator=(const CgiHandler&);

        
        std::string getHeader(const HttpRequest& request, const std::string& key);

    public:
        CgiHandler(const HttpRequest& request, const std::string& full_path);
        ~CgiHandler(void);

		int			        executeCgi(pid_t& pid);
		static std::string  process_response(const std::string& content);
};
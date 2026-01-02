#pragma once

#include <map>
#include <string>

#include "Parser.hpp"

// CGI -> Common Gateway Interface

class CgiHandler {
    private:
        std::map<std::string, std::string>  _env;
        std::string                         _body;

        CgiHandler(const CgiHandler&);
        CgiHandler& operator=(const CgiHandler&);

        
        std::string getHeader(const HttpRequest& request, const std::string& key);
        void        setScriptInfo(const std::string& route, const std::string& full_path, const std::string& cgi_ext);

    public:
        CgiHandler(const HttpRequest& request, const std::string& full_path, std::pair<std::string, std::string> cgi_extension);
        ~CgiHandler(void);

		int			        executeCgi(pid_t& pid, std::pair<std::string, std::string>& cgi_extension);
		static std::string  process_response(const std::string& content);
};
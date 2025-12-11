#pragma once

// CGI -> Common Gateway Interface

// Receive HttpRequest
// Absolute PATH of Script to execute

#include "Parser.hpp"


class CgiHandler {
    private:
        std::map<std::string, std::string>  _env;


        CgiHandler(const CgiHandler&);
        CgiHandler& operator=(const CgiHandler&);

        std::string getHeader(const HttpRequest& request, const std::string& key);

    public:
        CgiHandler(const HttpRequest& request, const std::string& full_path);
        ~CgiHandler(void);
};
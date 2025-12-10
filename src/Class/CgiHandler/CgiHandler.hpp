#pragma once

// CGI -> Common Gateway Interface

// Receive HttpRequest
// Absolute PATH of Script to execute

#include "Parser.hpp"


class CgiHandler {
    private:
        CgiHandler(const CgiHandler&);
        CgiHandler& operator=(const CgiHandler&);

    public:
        CgiHandler(const HttpRequest& request, const std::string& full_path);
        ~CgiHandler(void);
};
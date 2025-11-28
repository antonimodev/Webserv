#pragma once

#include <string>

// Usual request: GET /index.html HTTP/1.1
struct HttpRequest {
	std::string method;
	std::string route;
	std::string version;
};

class Parser {
    private:
        void    validMethod(const std::string& method);
        void    validRoute(const std::string& method);
        void    validVersion(const std::string& method);
        void    validHttpRequest(const std::string& request, HttpRequest& httpStruct);

    public:
        Parser(void);
        ~Parser(void);

        static HttpRequest    parseHttpRequest(const std::string& request);
};
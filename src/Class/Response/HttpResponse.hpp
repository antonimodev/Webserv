#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>
#include <map>

class HttpResponse
{
	private:
	    int 								_status_code;
	    std::map<std::string, std::string> 	_headers;
	    std::string 						_body;
    	std::string 						getStatusMessage(int code) const;

	public:
	    HttpResponse();
	    ~HttpResponse();
	
	    void setStatus(int code);
	    void setHeader(const std::string& key, const std::string& value);
	    void setBody(const std::string& body);
	    std::string toString() const;
};

#endif

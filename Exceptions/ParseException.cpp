#include <sstream>

#include "ParseException.hpp"

HandleReceive
try () {
    
} catch (const ParseException& e) {
    std::cout << e.what() << std::endl;
    _client_map[client.fd].response_buffer = e.getStatus();
}

status_msg = "400 Bad Request";

status_msg = "404 Not Found";

std::string    ParseException::httpResponse(const std::string& status_msg) {
    std::string msg = "<html><body><h1>" + status_msg + "</h1></body></html>";
    std::ostringstream oss;
    oss << msg.size();

    std::string response =
        "HTTP/1.1 " + status_msg + "\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: " + oss.str() + "\r\n"
        "\r\n" + msg;

    return response;
}



/*
const char* ParseException::statusToString(HttpStatus s) {
    switch (s) {
        case BAD_REQUEST:         return "400 Bad Request";
        case FORBIDDEN:           return "403 Forbidden";
        case NOT_FOUND:           return "404 Not Found";
        case METHOD_NOT_ALLOWED:  return "405 Method Not Allowed";
        case INTERNAL_ERROR:      return "500 Internal Server Error";
    }
    return "500 Internal Server Error";
}

ParseException::ParseException(HttpStatus status, const std::string& msg)
    : std::runtime_error(msg.empty() ? statusToString(status) : msg), _status(status) {}

std::string ParseException::httpResponse() const {
    const char* status = statusToString(_status);
    std::ostringstream oss;
    oss << "<html><body><h1>" << status << "</h1></body></html>";
    std::string body = oss.str();
    
    std::ostringstream response;
    response << "HTTP/1.1 " << status << "\r\n"
             << "Content-Type: text/html\r\n"
             << "Content-Length: " << body.size() << "\r\n"
             << "\r\n" << body;
    return response.str();
}
*/
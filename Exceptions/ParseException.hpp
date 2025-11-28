#pragma once

#include <stdexcept>
#include <string>


class ParseException : public std::runtime_error {
	public:
		ParseException(const std::string& msg) : std::runtime_error(msg) {}
};

// STATUS CODES
class NotFoundException : public ParseException {
	public:
		NotFoundException(const std::string& msg) : ParseException(msg) {}
};

class BadRequestException : public ParseException {
	public:
		BadRequestException(const std::string& msg) : ParseException(msg) {}
};


/*
enum HttpStatus {
    BAD_REQUEST = 400,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    INTERNAL_ERROR = 500
};

class ParseException : public std::runtime_error {
private:
    HttpStatus _status;
    
    static const char* statusToString(HttpStatus s);

public:
    ParseException(HttpStatus status, const std::string& msg = "");
    std::string httpResponse() const;
};
*/
#include "ParseException.hpp"

ParseException::ParseException(const std::string& msg) : std::runtime_error(msg), _msg(msg) {}

ParseException::~ParseException(void) throw() {}

const char* ParseException::what(void) const throw() {
    return _msg.c_str();
}
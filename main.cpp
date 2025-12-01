#include <iostream>

#include "SocketException.hpp"
#include "PollException.hpp"

#include "Webserv.hpp"
#include "webserv.h"

int main(void) {
	try {
		Webserv webserv;
		webserv.runServer();
	} catch ( const SocketException& e ) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
}
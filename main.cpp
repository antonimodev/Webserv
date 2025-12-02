#include <iostream>

#include "SocketException.hpp"
#include "PollException.hpp"

#include "Webserv.hpp"
#include "Socket.hpp"
#include "webserv.h"


int main(void) {
	try {
		Webserv webserv;
		webserv.addSocket("127.0.0.1", 8080);
		webserv.runServer();
	} catch ( const SocketException& e ) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
}
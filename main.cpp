#include <iostream>

#include "SocketException.hpp"
#include "PollException.hpp"

#include "Webserv.hpp"
#include "Socket.hpp"
#include "webserv.h"


int main(int ac, char** av) {
	if (ac != 2) {
		std::cerr << "Usage: " << av[0] << " <config_file>" << std::endl;
		return 1;
	}

	try {
		Webserv webserv(av[1]);
		webserv.runServer();
	} catch ( const SocketException& e ) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
}
#include <arpa/inet.h>
#include <sys/socket.h>

#include <fcntl.h>

#include <unistd.h>

#include "SocketException.hpp"
#include "webserv.h"

// Create a socket
int create_socket(int domain, int type, int protocol) {
	int socket_fd = socket(domain, type, protocol);

	if (socket_fd == -1)
		throw SocketException("Error: socket failed to create");

	return socket_fd;
}

// Set a socket to non-blocking mode
void	set_nonblocking(int& socket_fd) {
	if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) == -1) {
		close(socket_fd);
		throw SocketException("Error: fcntl failed to set non-blocking mode");
	}
}

// Set socket options (like SO_REUSEADDR)
void	set_socket_mode(int& socket_fd, int mode) {
	int opt = 1;

	if (setsockopt(socket_fd, SOL_SOCKET, mode, &opt, sizeof(opt)) == -1) {
		close(socket_fd);
		throw SocketException("Error: setsockopt failed");
	}
}

// Bind socket to IP and Port
void	bind_socket(int& socket_fd, const char* ip, int port, int domain) {
	sockaddr_in address;
	address.sin_addr.s_addr = inet_addr(ip);
	address.sin_port = htons(port);
	address.sin_family = domain;

	if (bind(socket_fd, (const sockaddr *)&address, sizeof(address)) == -1) {
		close(socket_fd);
		throw SocketException("Error: bind failed");
	}
}

// Listen on the socket
void	listen_socket(int& socket_fd, int backlog) {
	if (listen(socket_fd, backlog) == -1) {
		close(socket_fd);
		throw SocketException("Error: listen failed");
	}
}

// MASTER FUNCTION TO INITIALIZE SERVER SOCKET
int init_server_socket(void) {
	int socket_fd;

	socket_fd = create_socket(AF_INET, SOCK_STREAM, 0);
	set_nonblocking(socket_fd);
	set_socket_mode(socket_fd, SO_REUSEADDR);
	bind_socket(socket_fd, "127.0.0.1", 8080, AF_INET);
	listen_socket(socket_fd, 5);

	return socket_fd;
}
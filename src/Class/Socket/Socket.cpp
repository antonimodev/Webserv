#include <arpa/inet.h>
#include <sys/socket.h>

#include <fcntl.h>
#include <unistd.h>

#include "SocketException.hpp"
#include "Socket.hpp"


Socket::Socket(const std::string& ip, int port) : _fd(-1) {
	_fd = createSocket(AF_INET, SOCK_STREAM, 0);
	setNonBlocking(_fd);
	setSocketMode(_fd, SO_REUSEADDR);
	bindSocket(_fd, ip, port, AF_INET);
	listenSocket(_fd, 5);
}


Socket::~Socket(void) {
	if (_fd >= 0)
		close(_fd);
}


// PRIVATE
int	Socket::createSocket(int domain, int type, int protocol) {
	int socket_fd = socket(domain, type, protocol);
	if (socket_fd == -1)
		throw SocketException("Error: socket() failed to create");
	return socket_fd;
}


void	Socket::setSocketMode(int socket_fd, int mode) {
	int opt = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, mode, &opt, sizeof(opt)) == -1)
		throw SocketException("Error: setsockopt() failed");
}


void	Socket::bindSocket(int socket_fd, const std::string& ip, int port, int domain) {
	sockaddr_in address;
	address.sin_addr.s_addr = inet_addr(ip.c_str());
	address.sin_port = htons(port);
	address.sin_family = domain;
	if (bind(socket_fd, (const sockaddr *)&address, sizeof(address)) == -1)
		throw SocketException("Error: bind() failed");
}


void	Socket::listenSocket(int socket_fd, int backlog) {
	if (listen(socket_fd, backlog) == -1)
		throw SocketException("Error: listen() failed");
}


// PUBLIC
int Socket::getSocketFd(void) const {
	return _fd;
}


void	Socket::setNonBlocking(int socket_fd) {
	if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) == -1)
		throw SocketException("Error: fcntl() failed to set non-blocking mode");
}
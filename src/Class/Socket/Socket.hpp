#pragma once

#include <string>

/**
 * @brief RAII wrapper for a TCP server socket.
 * 
 * Creates, binds, and listens on a socket. Automatically closes on destruction.
 * Non-copyable to prevent double-close issues.
 */
class Socket {
	private:
		int 				_fd;
		const std::string	_ip;
		int 				_port;

		/**
		 * @brief Creates a socket file descriptor.
		 * @param domain Address family (AF_INET, AF_INET6).
		 * @param type Socket type (SOCK_STREAM, SOCK_DGRAM).
		 * @param protocol Protocol (0 for default).
		 * @return Valid socket file descriptor.
		 * @throws SocketException on failure.
		 */
		static int createSocket(int domain, int type, int protocol);

		/**
		 * @brief Sets socket options.
		 * @param socket_fd Socket file descriptor.
		 * @param mode Option to set (SO_REUSEADDR, etc).
		 * @throws SocketException on failure.
		 */
		static void	setSocketMode(int socket_fd, int mode);

		/**
		 * @brief Binds socket to address and port.
		 * @param socket_fd Socket file descriptor.
		 * @param ip IP address as string.
		 * @param port Port number.
		 * @param domain Address family.
		 * @throws SocketException on failure.
		 */
		static void	bindSocket(int socket_fd, const std::string& ip, int port, int domain);

		/**
		 * @brief Starts listening for connections.
		 * @param socket_fd Socket file descriptor.
		 * @param backlog Maximum pending connections queue size.
		 * @throws SocketException on failure.
		 */
		static void	listenSocket(int socket_fd, int backlog);

		// Disable copy to prevent double-close (Rule of Three)
		Socket(const Socket&);
		Socket& operator=(const Socket&);

	public:
		/**
		 * @brief Constructs and initializes a listening socket.
		 * @param ip IP address to bind.
		 * @param port Port to bind.
		 * @throws SocketException on any socket operation failure.
		 */
		Socket(const std::string& ip, int port);

		/**
		 * @brief Destructor. Closes the socket if valid.
		 */
		~Socket(void);

		/**
		 * @brief Gets the socket file descriptor.
		 * @return Socket file descriptor.
		 */
		int getSocketFd(void) const;

		int getPort(void) const;

		const std::string getIp(void) const;

		/**
		 * @brief Sets a socket to non-blocking mode.
		 * @param socket_fd Socket file descriptor.
		 * @throws SocketException on failure.
		 */
		static void	setNonBlocking(int socket_fd);
};
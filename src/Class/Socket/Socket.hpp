#pragma once


class Socket {
	private:
		int _fd;

		int     	createSocket(int domain, int type, int protocol);
		void    	setSocketMode(int socket_fd, int mode);

		void    	bindSocket(int fd, const std::string& ip, int port, int domain);
		void    	listenSocket(int fd, int backlog);

	public:
		Socket(const std::string& ip, int port);
		~Socket(void);

		int			getSocketFd(void) const;
		static void	setNonBlocking(int socket_fd);
};
#pragma once

// CONTAINERS
#include <vector>
#include <map>

//
#include <poll.h> // poll()

//
#include <sstream> // std::ostringstream

#include <ctime> // time_t

#include "webserv.h"
#include "Parser.hpp"
#include "Socket.hpp"
#include "ConfParser.hpp"


/**
 * @brief Holds the state of a connected client.
 */
struct ClientState {
	ServerConfig* _server_config; // added: pointer to the server the client came from

	HttpRequest	_http_request;    // Parsed HTTP request

	std::string	_request_buffer;  // Stores the incoming request piece by piece
	std::string	_response_buffer; // Stores the final response to be sent
	bool		_response_ready;  // Flag: true when ready to send

	time_t		_last_active;     // Last activity timestamp

	// CGI
	pid_t		_cgi_pid;
	int			_cgi_pipe_fd;

	ClientState(void) : _response_ready(false), _last_active(time(NULL)), _cgi_pid(-1), _cgi_pipe_fd(-1) {}
};


/**
 * @brief Main server class managing sockets, polling, and client connections.
 */
class Webserv {
	private:
		std::vector<Socket*>		_server_sockets;
		std::vector<ServerConfig>	_servers;
		std::vector<struct pollfd>	_poll_vector;
		std::map<int, ClientState>	_client_map;
		std::map<int, int>			_cgi_to_client;

		/**
		 * @brief Creates a pollfd struct for a socket.
		 * @param fd_socket File descriptor.
		 * @param event Events to monitor.
		 * @return Configured pollfd struct.
		 */
		static pollfd createPollfd(int fd_socket, short event);

		void		watchPollEvents(void);
		void		addPollEvent(int sock_fd, short event);

		void		handleNewConnection(int socket_fd);
		void		handleReceiveEvent(size_t& idx);
		void		handleCgiEvent(size_t& idx); // added
		void		handleSendEvent(size_t& idx);

		void		checkTimeout(void);
		void		resetClientInfo(int socket_fd);

		/**
		 * @brief Checks if a file descriptor belongs to a server socket.
		 * @param fd File descriptor to check.
		 * @return true if fd is a server socket.
		 */
		bool		isServerSocket(int fd) const;
		bool		isCgiRequest(const std::string& full_path, const std::pair<std::string, std::string>& cgi_extension);

		void		cleanupCgiProcess(int client_fd);

		void		disconnectClient(size_t& idx);
		void		processClientRequest(size_t& idx);
		void		setPollEvent(int fd, short event);

		// Disable copy (Rule of Three - Webserv manages dynamic resources)
		Webserv(const Webserv&);
		Webserv& operator=(const Webserv&);

		// added
		ServerConfig* getServerBySocketFd(int socket_fd);

	public:
		Webserv(void);
		Webserv(const char* av);
		Webserv& operator=(Webserv& other);
		~Webserv(void);

		void	runServer(void);
		void	addSocket(const std::string& ip, int port);
};
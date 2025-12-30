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
	int             _server_socket_fd;
	ServerConfig*   _server_config;
	HttpRequest     _http_request;
	std::string     _request_buffer;
	std::string     _response_buffer;
	bool            _response_ready;
	time_t          _last_active;
	pid_t           _cgi_pid;
	int             _cgi_pipe_fd;

	ClientState();  // ← Añadir esta declaración
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
		bool		isCgiRequest(const std::string& full_path);

		void		cleanupCgiProcess(int client_fd);

		void		disconnectClient(size_t& idx);
		void		processClientRequest(size_t& idx);
		void		setPollEvent(int fd, short event);

		// added
		//ServerConfig* getServerBySocketFd(int socket_fd);
		ServerConfig* getServerByHost(int server_socket_fd, std::string& host_header);
		ServerConfig* getClientServerConfig(const HttpRequest& request, ClientState& client);

		// Disable copy (Rule of Three - Webserv manages dynamic resources)
		Webserv(const Webserv&);
		Webserv& operator=(const Webserv&);

	public:
		Webserv(void);
		Webserv(const char* av);
		Webserv& operator=(Webserv& other);
		~Webserv(void);

		void	runServer(void);
		void	addSocket(const std::string& ip, int port);
};
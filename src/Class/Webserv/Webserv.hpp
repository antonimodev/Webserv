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
	std::string	_request_buffer;  // Stores the incoming request piece by piece
	std::string	_response_buffer; // Stores the final response to be sent
	bool		_response_ready;  // Flag: true when ready to send

	time_t		_last_active;     // Last activity timestamp

	HttpRequest	_http_request;    // Parsed HTTP request

	ClientState(void) : _response_ready(false), _last_active(time(NULL)) {}
};


/**
 * @brief Main server class managing sockets, polling, and client connections.
 */
class Webserv {
	private:
		std::vector<Socket*>		_server_sockets;	// Created as pointer to preserve class outside scope of his own creation
		std::vector<struct pollfd>	_poll_vector;
		std::map<int, ClientState>	_client_map;
		std::vector<ServerConfig>	_servers;


		pollfd	create_struct_pollfd(int fd_socket, short event);
		void	watchPollEvents();
		void	addPollEvent(int sock_fd, short event);

		void	handleNewConnection(int socket_fd);
		void	handleReceiveEvent(size_t& idx);
		void	handleSendEvent(size_t& idx);

		void	checkTimeout(void);
		void	resetClientInfo(int socket_fd);
		bool	isServerSocket(int fd) const;

		void	disconnectClient(size_t& idx);
		void	processClientRequest(size_t& idx);

	public:
		Webserv(const char* conf_file);
		~Webserv(void);

		void	runServer(void);
		void	addSocket(std::string& ip, int port);
};
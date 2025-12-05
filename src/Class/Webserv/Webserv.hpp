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
//#include "ConfParser.hpp"
#include "../ConfParser/ConfParser.hpp"


struct ClientState {
	std::string request_buffer;  // Stores the incoming request piece by piece
	std::string response_buffer; // Stores the final response to be sent
	bool        response_ready;  // Flag: true when we have finished processing and are ready to send

	time_t		last_active;

	HttpRequest	http_request;

	ClientState() : response_ready(false), last_active(time(NULL)) {}
};


class Webserv {
	private:
		std::vector<Socket*>		_server_sockets;	// Created as pointer to preserve class outside scope of his own creation
		std::vector<struct pollfd>	_poll_vector;
		std::map<int, ClientState>	_client_map;

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
		Webserv(void);
		~Webserv(void);

		void	runServer(void);
		void	addSocket(const char* ip, int port);
};
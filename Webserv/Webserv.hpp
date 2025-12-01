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
		int							_tcp_socket;
		std::vector<struct pollfd>	_poll_vector;
		std::map<int, ClientState>	_client_map;

		pollfd	create_struct_pollfd(int& fd_socket, short event);
		void	watchPollEvents();
		void	addPollEvent(int& sock_fd, short event);
		void	handleNewConnection(void);
		void	handleReceiveEvent(size_t& idx);
		void	handleSendEvent(size_t& idx);
		void	checkTimeout(void);
		void	resetClientInfo(int& socket_fd);

	public:
		Webserv(void);
		~Webserv(void);

		void	runServer(void);
};
#pragma once

// CONTAINERS
#include <vector>
#include <map>

//
#include <poll.h> // poll()

//
#include <sstream> // std::ostringstream

#include "webserv.h"

struct ClientState {
	std::string request_buffer;  // Stores the incoming request piece by piece
	std::string response_buffer; // Stores the final response to be sent
	bool        response_ready;  // Flag: true when we have finished processing and are ready to send

	ClientState() : response_ready(false) {}
};

class Webserv {
	private:
		std::vector<struct pollfd>	_poll_vector;
		std::map<int, ClientState>	_client_map;
		int							_tcp_socket;

		pollfd	create_struct_pollfd(int& fd_socket, short event);
		void	watchPollEvents();
		void	addPollEvent(int& sock_fd, short event);
		void	handleNewConnection(void);
		void	handleReceiveEvent(size_t& idx);
		void	handleSendEvent(size_t& idx);

	public:
		Webserv(void);
		~Webserv(void);

		void	runServer(void);
};
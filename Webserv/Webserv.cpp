#include <sys/socket.h>
#include <arpa/inet.h>

#include <iostream>
#include <sstream>

#include <poll.h>

#include <unistd.h>

#include "Webserv.hpp"
#include "SocketException.hpp"
#include "PollException.hpp"

Webserv::Webserv(void) : _tcp_socket(init_server_socket()) {}

Webserv::~Webserv(void) {}

// PRIVATE
pollfd	Webserv::create_struct_pollfd(int& fd_socket, short event) {
	pollfd pollfd;

	pollfd.fd = fd_socket;
	pollfd.events = event;
	pollfd.revents = 0;

	return pollfd;
}

void	Webserv::watchPollEvents() {
	int result;
	
	result = poll(_poll_vector.data(), _poll_vector.size(), 5000);

	if (result == -1)
		throw PollException("Error: poll() failed");

	if (result == 0)
		throw PollException("Error: poll() timeout");
}

void	Webserv::addPollEvent(int& sock_fd, short event) {
	_poll_vector.push_back(create_struct_pollfd(sock_fd, event));
}

void	Webserv::handleNewConnection() {
	sockaddr_in address;
	socklen_t socklen = sizeof(address);
	int agent = accept(_tcp_socket, (sockaddr *)&address, &socklen);

	if (agent == -1) {
		std::cerr << "agent -1" << std::endl;
		return;
	}
	set_nonblocking(agent);
	
	if (agent >= 0) {
		addPollEvent(agent, POLLIN);
		_client_map[agent] = ClientState();
		std::cout << "New client connected: " << agent << std::endl;
	}
}

// Modularize in the future
void	Webserv::handleReceiveEvent(size_t& idx) {
	int client_fd = _poll_vector[idx].fd;
	char buffer[1024];
	ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes_read <= 0) {
		close(client_fd);
		_client_map.erase(client_fd);
		_poll_vector.erase(_poll_vector.begin() + idx);
		--idx;
		std::cout << "Client disconnected" << std::endl;
	} else {
		buffer[bytes_read] = '\0';
		_client_map[client_fd].request_buffer += buffer;

		if (_client_map[client_fd].request_buffer.find("\r\n\r\n") != std::string::npos) {
			// Prepare the response (store it in the map, don't send yet!)
			std::string msg = "<h1>Hello from Webserv</h1>";
			std::ostringstream oss;
			oss << msg.size();

			_client_map[client_fd].response_buffer =
				"HTTP/1.1 200 OK\r\n"
				"Content-Type: text/html\r\n"
				"Content-Length: " + oss.str() + "\r\n"
				"\r\n" + msg;

			_client_map[client_fd].response_ready = true;

			_poll_vector[idx].events = POLLOUT;
		}
	}
}

void	Webserv::handleSendEvent(size_t& idx) {
	int client_fd = _poll_vector[idx].fd;

	if (_client_map[client_fd].response_ready) {
		std::string& response = _client_map[client_fd].response_buffer;
		
		ssize_t bytes_sent = send(client_fd, response.c_str(), response.size(), 0);
		
		if (bytes_sent >= 0) {
			// For simplicity: assume full send. In real webserv, handle partial sends.
			std::cout << "Response sent to " << client_fd << std::endl;
			
			// Close connection (HTTP 1.0 style)
			close(client_fd);
			_client_map.erase(client_fd);
			_poll_vector.erase(_poll_vector.begin() + idx);
			--idx;
		}
	}
}

void	Webserv::runServer(void) {
	addPollEvent(_tcp_socket, POLLIN);

	while (true) {
		try {
			watchPollEvents();
		} catch (const PollException& e) {
			std::cerr << e.what() << std::endl;
			continue;
		}
		// Iterate through all sockets
		for (size_t i = 0; i < _poll_vector.size(); ++i) {
			if (_poll_vector[i].fd == _tcp_socket && (_poll_vector[i].revents & POLLIN))
				handleNewConnection();
			else if (_poll_vector[i].revents & POLLIN)
				handleReceiveEvent(i);
			else if (_poll_vector[i].revents & POLLOUT)
				handleSendEvent(i);
	}
	close(_tcp_socket);
	}
}

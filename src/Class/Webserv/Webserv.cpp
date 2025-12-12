#include <sys/socket.h>
#include <netinet/in.h>

#include <iostream>
#include <unistd.h>

#include <sys/stat.h>

#include <cstdio>

#include "webserv.h"
#include "Webserv.hpp"
#include "Socket.hpp"
#include "../ConfParser/ConfParser.hpp"

#include "PollException.hpp"
#include "HttpCodeException.hpp"
#include "SocketException.hpp"
#include "PendingRequestException.hpp"


Webserv::Webserv(const char* conf_file) {
	ConfParser parser(conf_file);
	_servers = parser.getServers();

	//webserv.addSocket("127.0.0.1", 8080);
	//addSocket(_servers[0].host, _servers[0].listen_port);

	for (size_t i = 0; i < _servers.size(); ++i) {
        addSocket(_servers[i].host, _servers[i].listen_port);
	}
}


// Disabled copy operations
Webserv::Webserv(const Webserv& other) {
	(void)other;
}


Webserv& Webserv::operator=(const Webserv& other) {
	(void)other;
	return *this;
}


Webserv::~Webserv(void) {
	for (size_t i = 0; i < _server_sockets.size(); ++i)
		delete _server_sockets[i];

	_server_sockets.clear();
}


// PRIVATE STATIC


pollfd Webserv::createPollfd(int socket_fd, short event) {
	pollfd pfd;

	pfd.fd = socket_fd;
	pfd.events = event;
	pfd.revents = 0;

	return pfd;
}


void	Webserv::watchPollEvents(void) {
	int result;
	
	result = poll(_poll_vector.data(), _poll_vector.size(), 5000);

	if (result == -1)
		throw PollException("Error: poll() failed");

	if (result == 0)
		std::cout << "State: poll() timeout" << std::endl;
}


void	Webserv::addPollEvent(int sock_fd, short event) {
	_poll_vector.push_back(createPollfd(sock_fd, event));
}


void	Webserv::checkTimeout(void) {
	time_t current_time = time(NULL);

	for (size_t i = 1; i < _poll_vector.size(); ++i) {
		int client_fd = _poll_vector[i].fd;

		if ((current_time - _client_map[client_fd]._last_active) > 30) {
			close(client_fd);
			_client_map.erase(client_fd);
			_poll_vector.erase(_poll_vector.begin() + i);
			--i;
			std::cout << "Client " << client_fd << " has been disconnected due inactivity" << std::endl;
		}
	}
}


void	Webserv::resetClientInfo(int socket_fd) {
	_client_map[socket_fd]._request_buffer.clear();
	_client_map[socket_fd]._response_buffer.clear();
	_client_map[socket_fd]._response_ready = false;
}


bool	Webserv::isServerSocket(int fd) const {
	for (size_t i = 0; i < _server_sockets.size(); ++i) {
		if (_server_sockets[i]->getSocketFd() == fd)
			return true;
	}
	return false;
}


void	Webserv::addSocket(std::string& ip, int port) {
	Socket* new_socket = new Socket(ip, port);

	_server_sockets.push_back(new_socket);
}



void	Webserv::disconnectClient(size_t& idx) {
	int client_fd = _poll_vector[idx].fd;

	close(client_fd);
	_client_map.erase(client_fd);
	_poll_vector.erase(_poll_vector.begin() + idx);
	--idx;
	std::cout << "Client disconnected" << std::endl;
}


void	Webserv::processClientRequest(size_t& idx) { 
	const int client_fd = _poll_vector[idx].fd;

	try {
		_client_map[client_fd]._http_request = 
			Parser::parseHttpRequest(_client_map[client_fd]._request_buffer);

		const std::string route = _client_map[client_fd]._http_request.route;
		const std::string method = _client_map[client_fd]._http_request.method;
		const std::string base_path = "./static";
		const std::string full_path = base_path + route;
		const std::string body = _client_map[client_fd]._http_request.body;

		if (method == "GET")
			_client_map[client_fd]._response_buffer = load_resource(full_path, route);
		else if (method == "DELETE")
			_client_map[client_fd]._response_buffer = delete_resource(full_path);
		else if (method == "POST")
			_client_map[client_fd]._response_buffer = save_resource(full_path, body);

	} catch (const PendingRequestException& e) {
		std::cout << "Client " << client_fd << ": " << e.what() << std::endl;
		return;
	} catch (const HttpCodeException& e) {
		std::cerr << e.what() << std::endl;
		_client_map[client_fd]._response_buffer = e.httpResponse();
	}

	_client_map[client_fd]._response_ready = true;
	_poll_vector[idx].events = POLLOUT;
}


// HANDLE EVENTS AND CONNECTIONS


void	Webserv::handleNewConnection(int socket_fd) {
	sockaddr_in address;
	socklen_t socklen = sizeof(address);
	int agent = accept(socket_fd, reinterpret_cast<sockaddr*>(&address), &socklen);

	if (agent == -1)
		throw SocketException("Error: accept() failed");

	Socket::setNonBlocking(agent);

	if (agent >= 0) {
		addPollEvent(agent, POLLIN);
		_client_map[agent] = ClientState();
		std::cout << "New client connected: " << agent << std::endl;
	}
}


void	Webserv::handleReceiveEvent(size_t& idx) {
	int client_fd = _poll_vector[idx].fd;
	char buffer[1024];
	ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes_read <= 0)
		return disconnectClient(idx);

	_client_map[client_fd]._last_active = time(NULL);
	buffer[bytes_read] = '\0';
	_client_map[client_fd]._request_buffer.append(buffer);

	if (_client_map[client_fd]._request_buffer.find("\r\n\r\n") != std::string::npos) {
		processClientRequest(idx);

		_client_map[client_fd]._response_ready = true;
		_poll_vector[idx].events = POLLOUT;
	}
}


void	Webserv::handleSendEvent(size_t& idx) {
	int client_fd = _poll_vector[idx].fd;

	if (_client_map[client_fd]._response_ready) {
		std::string& response = _client_map[client_fd]._response_buffer;
		
		ssize_t bytes_sent = send(client_fd, response.c_str(), response.size(), 0);

		if (bytes_sent >= 0) {
			resetClientInfo(client_fd);
			_client_map[client_fd]._last_active = time(NULL);
			_poll_vector[idx].events = POLLIN;
		}
	}
}


// CORE FUNCTION


void	Webserv::runServer(void) {
	for (size_t i = 0; i < _server_sockets.size(); ++i)
		addPollEvent(_server_sockets[i]->getSocketFd(), POLLIN);

	while (true) {
		try {
			watchPollEvents();
		} catch (const PollException& e) {
			std::cerr << e.what() << std::endl;
			continue;
		}

		checkTimeout();

		for (size_t i = 0; i < _poll_vector.size(); ++i) {
			int fd = _poll_vector[i].fd;

			if (_poll_vector[i].revents & POLLIN) {
				if (isServerSocket(fd))
					handleNewConnection(fd);
				else
					handleReceiveEvent(i);
			}
			else if (_poll_vector[i].revents & POLLOUT)
				handleSendEvent(i);
		}
	}
}
#include <sys/socket.h>
#include <netinet/in.h>

#include <iostream>
#include <unistd.h>

#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <cstdio>
#include <map>

#include "webserv.h"
#include "Webserv.hpp"
#include "Socket.hpp"
#include "CgiHandler.hpp"
#include "ConfParser.hpp"

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
	struct pollfd pfd;

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


void	Webserv::addSocket(const std::string& ip, int port) {
	Socket* new_socket = new Socket(ip, port);

	_server_sockets.push_back(new_socket);
}


bool	Webserv::isServerSocket(int fd) const {
	for (size_t i = 0; i < _server_sockets.size(); ++i) {
		if (_server_sockets[i]->getSocketFd() == fd)
			return true;
	}
	return false;
}


void	Webserv::disconnectClient(size_t& idx) {
	int client_fd = _poll_vector[idx].fd;

	close(client_fd);
	_client_map.erase(client_fd);
	_poll_vector.erase(_poll_vector.begin() + idx);
	--idx;
	std::cout << "Client disconnected" << std::endl;
}


std::string	Webserv::handleStaticRequest(const HttpRequest& request, const std::string& full_path) {
	if (request.method == "GET")
		return load_resource(full_path, request.route);
	else if (request.method == "DELETE")
		return delete_resource(full_path);
	else if (request.method == "POST")
		return save_resource(full_path, request.body);

	return "";
}


bool	Webserv::isCgiRequest(const std::string& full_path) {
	std::string extension = get_extension(full_path);

	if (extension == "php" || extension == "py")
		return true;

	return false;
}


void	Webserv::cleanupCgiProcess(int client_fd) {
	int	pipe_fd = _client_map[client_fd]._cgi_pipe_fd;

	if (pipe_fd != -1) {
		_cgi_to_client.erase(pipe_fd);
		close(pipe_fd);
	}

	waitpid(_client_map[client_fd]._cgi_pid, NULL, WNOHANG);

	_client_map[client_fd]._cgi_pipe_fd = -1;
	_client_map[client_fd]._cgi_pid = -1;
}


void	Webserv::processClientRequest(size_t& idx) { 
	const int client_fd = _poll_vector[idx].fd;

	try {
		_client_map[client_fd]._http_request = Parser::parseHttpRequest(_client_map[client_fd]._request_buffer);

		const HttpRequest& request = _client_map[client_fd]._http_request;
		const std::string full_path = "./static" + request.route;

		if (isCgiRequest(full_path)) {
			CgiHandler cgi(request, full_path);

			int	pipe_fd = cgi.executeCgi(_client_map[client_fd]._cgi_pid);
			_client_map[client_fd]._cgi_pipe_fd = pipe_fd;

			_cgi_to_client[pipe_fd] = client_fd; // testing
			addPollEvent(pipe_fd, POLLIN);
			return ; // avoid response at this point
		}

		_client_map[client_fd]._response_buffer = handleStaticRequest(request, full_path);

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
	struct sockaddr_in address;
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


void Webserv::handleCgiEvent(size_t& idx) {
	int pipe_fd = _poll_vector[idx].fd;

	if (_cgi_to_client.find(pipe_fd) == _cgi_to_client.end()) {
		_poll_vector.erase(_poll_vector.begin() + idx);
		--idx;
		return;
	}

	int client_fd = _cgi_to_client[pipe_fd];

	try {
		ssize_t bytes_read = readFd(pipe_fd, _client_map[client_fd]._response_buffer, PIPE);

		if (bytes_read > 0) {
			_client_map[client_fd]._last_active = time(NULL);

			return; 
		}

		if (bytes_read == 0) {
			_client_map[client_fd]._response_buffer = CgiHandler::process_response(_client_map[client_fd]._response_buffer);
			
			cleanupCgiProcess(client_fd);
			_client_map[client_fd]._response_ready = true;

			for (size_t i = 0; i < _poll_vector.size(); ++i) {
				if (_poll_vector[i].fd == client_fd) {
					_poll_vector[i].events = POLLOUT;
					break;
				}
			}

			_poll_vector.erase(_poll_vector.begin() + idx);
			--idx;
		}
		else
			throw HttpCodeException(INTERNAL_ERROR, "Error: CGI pipe read error");
	} catch (const HttpCodeException& e) {
		std::cerr << e.what() << std::endl;
		_client_map[client_fd]._response_buffer = e.httpResponse();

		cleanupCgiProcess(client_fd);
		_client_map[client_fd]._response_ready = true;

		for (size_t i = 0; i < _poll_vector.size(); ++i) {
			if (_poll_vector[i].fd == client_fd) {
				_poll_vector[i].events = POLLOUT;
				break;
			}
		}
		_poll_vector.erase(_poll_vector.begin() + idx);
		--idx;
	}
}


void	Webserv::handleReceiveEvent(size_t& idx) {
	int client_fd = _poll_vector[idx].fd;
	ssize_t bytes_read = readFd(client_fd, _client_map[client_fd]._request_buffer, SOCKET);

	if (bytes_read <= 0)
		return disconnectClient(idx);

	_client_map[client_fd]._last_active = time(NULL);

	if (_client_map[client_fd]._request_buffer.find("\r\n\r\n") != std::string::npos)
		processClientRequest(idx);
}


void	Webserv::handleSendEvent(size_t& idx) {
	int client_fd = _poll_vector[idx].fd;

	if (_client_map[client_fd]._response_ready) {
		std::string& response = _client_map[client_fd]._response_buffer;
		
		ssize_t bytes_sent = send(client_fd, response.c_str(), response.size(), 0);
		if (bytes_sent == -1) {
			disconnectClient(idx);
			return ;
		}

		response.erase(0, bytes_sent);

		if (response.empty()) {
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

			if (_poll_vector[i].revents & (POLLIN | POLLHUP)) {
				if (isServerSocket(fd))
					handleNewConnection(fd);
				else if (_cgi_to_client.count(fd))
					handleCgiEvent(i);
				else
					handleReceiveEvent(i);
			}
			else if (_poll_vector[i].revents & POLLOUT)
				handleSendEvent(i);
		}
	}
}
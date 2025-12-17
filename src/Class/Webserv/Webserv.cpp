#include <sys/socket.h>
#include <netinet/in.h>

#include <iostream>
#include <unistd.h>

#include <sys/stat.h>

#include <cstdio>

#include "webserv.h"
#include "Webserv.hpp"
#include "Socket.hpp"
#include "ConfParser.hpp"

#include "PollException.hpp"
#include "HttpCodeException.hpp"
#include "SocketException.hpp"
#include "PendingRequestException.hpp"

#include <algorithm> // added for std::find to find in vector


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

static LocationConfig* findBestLocation(const std::string& route, ServerConfig* config) {
	std::string best_match = "";
	LocationConfig* best_location = NULL;
	
	std::map<std::string, LocationConfig>::iterator it;
	for (it = config->locations.begin(); it != config->locations.end(); ++it) {
		const std::string& path = it->first;
		if (route.compare(0, path.length(), path) == 0) {
			if (path.length() > best_match.length()) {
				best_match = path;
				best_location = &(it->second);
			}
		}
	}
	return best_location;
}

static bool isMethodAllowed(const std::string& method, const std::vector<std::string>& allowed) {
	return std::find(allowed.begin(), allowed.end(), method) != allowed.end();
}


// added: new changes
void	Webserv::processClientRequest(size_t& idx) {
    const int client_fd = _poll_vector[idx].fd;

    try {
        ClientState& client_state = _client_map[client_fd];
        
        // -------- 
        client_state._http_request = 
            Parser::parseHttpRequest(client_state._request_buffer);

        const std::string& route = client_state._http_request.route;
        const std::string& method = client_state._http_request.method;
        const std::string& body = client_state._http_request.body;
        // --------


        // -------- validate content_lenght < client_max_body_size
        std::map<std::string, std::string>::const_iterator content_length_it = 
        client_state._http_request.headers.find("Content-Length");
        
        if (content_length_it != client_state._http_request.headers.end()) {
            std::istringstream iss(content_length_it->second);
            size_t content_length;
            iss >> content_length;
            
            if (content_length > client_state._server_config->client_max_body_size) {
                throw HttpCodeException(BAD_REQUEST, "Request body too large");
            }
        }

        LocationConfig* location = findBestLocation(route, client_state._server_config);

        // -------- build full path with location->root or server->root
        std::string base_path;
        std::string remaining_path;
    
        if (location != NULL) {
            if (location->root.empty())
                base_path = client_state._server_config->root;
            else
                base_path = location->root;

            remaining_path = route.substr(location->path.length());
        } else {
            base_path = client_state._server_config->root;
            remaining_path = route;
        }

        std::string full_path = base_path + remaining_path;


        // -------- ensure there is separator between base path and remaining path
        if (!base_path.empty() && base_path[base_path.length() - 1] != '/' && 
            !remaining_path.empty() && remaining_path[0] != '/') {
            full_path = base_path + "/" + remaining_path;
        }

        if (location != NULL && !isMethodAllowed(method, location->allowed_methods)) {
            throw HttpCodeException(METHOD_NOT_ALLOWED, "Method not allowed");
        }


        // -------- process method requested
        if (method == "GET") {
			// -------- determine index file (location->index or server->index)
			std::string index_file = (location != NULL && !location->index.empty()) ? location->index : client_state._server_config->index;
            client_state._response_buffer = load_resource(full_path, route, index_file);
		}
        else if (method == "DELETE")
            client_state._response_buffer = delete_resource(full_path);
        else if (method == "POST")
            client_state._response_buffer = save_resource(full_path, body);

    } catch (const PendingRequestException& e) {
        std::cout << "Client " << client_fd << ": " << e.what() << std::endl;
        return;
    } catch (const HttpCodeException& e) {
    	std::cerr << e.what() << std::endl;
    	_client_map[client_fd]._response_buffer = e.httpResponse(_client_map[client_fd]._server_config);
    }
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
		_client_map[agent]._server_config = getServerBySocketFd(socket_fd); // added
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

// added
ServerConfig* Webserv::getServerBySocketFd(int socket_fd) {
	for (size_t i = 0; i < _server_sockets.size(); ++i) {
		if (_server_sockets[i]->getSocketFd() == socket_fd) {
			return &_servers[i];
		}
	}
	return NULL;
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
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
#include "ParseException.hpp"

#include <algorithm>


Webserv::Webserv(const char* conf_file) {
	ConfParser parser(conf_file);
	_servers = parser.getServers();

	for (size_t i = 0; i < _servers.size(); ++i)
		addSocket(_servers[i].host, _servers[i].listen_port);
}


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


// ═══════════════════════════════════════════════════════════════════
// PRIVATE HELPERS
// ═══════════════════════════════════════════════════════════════════


pollfd Webserv::createPollfd(int socket_fd, short event) {
	struct pollfd pfd;

	pfd.fd = socket_fd;
	pfd.events = event;
	pfd.revents = 0;

	return pfd;
}


void	Webserv::addPollEvent(int sock_fd, short event) {
	_poll_vector.push_back(createPollfd(sock_fd, event));
}


void	Webserv::setPollEvent(int fd, short event) {
	for (size_t i = 0; i < _poll_vector.size(); ++i) {
		if (_poll_vector[i].fd == fd) {
			_poll_vector[i].events = event;
			return;
		}
	}
}


void	Webserv::watchPollEvents(void) {
	int result = poll(_poll_vector.data(), _poll_vector.size(), 5000);

	if (result == -1)
		throw PollException("Error: poll() failed");

	if (result == 0)
		std::cout << "State: poll() timeout" << std::endl;
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


bool	Webserv::isCgiRequest(const std::string& full_path, const std::pair<std::string, std::string>& cgi_extension) {
	std::string extension = get_extension(full_path);
	return (extension == cgi_extension.first);
}


ServerConfig* Webserv::getServerBySocketFd(int socket_fd) {
	for (size_t i = 0; i < _server_sockets.size(); ++i) {
		if (_server_sockets[i]->getSocketFd() == socket_fd)
			return &_servers[i];
	}
	return NULL;
}


void	Webserv::disconnectClient(size_t& idx) {
	int client_fd = _poll_vector[idx].fd;

	close(client_fd);
	_client_map.erase(client_fd);
	_poll_vector.erase(_poll_vector.begin() + idx);
	--idx;
	std::cout << "Client disconnected" << std::endl;
}


void	Webserv::cleanupCgiProcess(int client_fd) {
	int pipe_fd = _client_map[client_fd]._cgi_pipe_fd;

	if (pipe_fd != -1) {
		_cgi_to_client.erase(pipe_fd);
		close(pipe_fd);
	}

	waitpid(_client_map[client_fd]._cgi_pid, NULL, WNOHANG);

	_client_map[client_fd]._cgi_pipe_fd = -1;
	_client_map[client_fd]._cgi_pid = -1;
}


void	Webserv::checkTimeout(void) {
	time_t current_time = time(NULL);

	for (size_t i = _poll_vector.size(); i > 0; --i) {
		size_t idx = i - 1;
		int fd = _poll_vector[idx].fd;

		// Skip server sockets and CGI pipes
		if (isServerSocket(fd) || _cgi_to_client.count(fd))
			continue;

		if (_client_map.find(fd) == _client_map.end())
			continue;

		if ((current_time - _client_map[fd]._last_active) > 30) {
			close(fd);
			_client_map.erase(fd);
			_poll_vector.erase(_poll_vector.begin() + static_cast<long>(idx));
			std::cout << "Client " << fd << " disconnected due to inactivity" << std::endl;
		}
	}
}

// ═══════════════════════════════════════════════════════════════════
// REQUEST PROCESSING HELPERS (static)
// ═══════════════════════════════════════════════════════════════════


static LocationConfig* findLocation(const std::string& route, ServerConfig* config) {
	std::string match;
	LocationConfig* location = NULL;

	std::map<std::string, LocationConfig>::iterator it = config->locations.begin();

	for (; it != config->locations.end(); ++it) {
		const std::string& path = it->first;

		if (route.compare(0, path.length(), path) == 0) {
			if (path.length() > match.length()) {
				match = path;
				location = &(it->second);
			}
		}
	}

	return location;
}


static bool isAllowedMethod(const std::string& method, const std::vector<std::string>& allowed) {
	return std::find(allowed.begin(), allowed.end(), method) != allowed.end();
}


static void validateBodySize(const HttpRequest& request, const ServerConfig* config) {
	if (config->client_max_body_size == 0)
		return; // No limit configured

	// Body already parsed by Parser, just check size
	if (request.body.size() > config->client_max_body_size)
		throw HttpCodeException(BAD_REQUEST, "Request body too large");
}


static std::string buildFullPath(const std::string& route, LocationConfig* location, ServerConfig* config) {
	std::string base_path;
	std::string remaining_path;

	if (location != NULL) {
		base_path = location->root.empty() ? config->root : location->root;
		remaining_path = route.substr(location->path.length());
	} else {
		base_path = config->root;
		remaining_path = route;
	}

	// Ensure separator between paths
	if (!base_path.empty() && base_path[base_path.length() - 1] != '/' &&
		!remaining_path.empty() && remaining_path[0] != '/')
		return base_path + "/" + remaining_path;

	return base_path + remaining_path;
}


static std::string getIndexFile(LocationConfig* location, ServerConfig* config) {
	if (location != NULL && !location->index.empty())
		return location->index;
	return config->index;
}

static std::string extract_filename(const std::string& route) {
	size_t pos = route.find_last_of('/');
	if (pos == std::string::npos)
		return route;
	return route.substr(pos + 1);
}

static std::string handleStaticRequest(const HttpRequest& request, LocationConfig* location, ServerConfig* config) {
	const std::string& method = request.method;
	std::string full_path = buildFullPath(request.route, location, config);

	if (method == "GET") {
		std::string index_file = getIndexFile(location, config);
		return load_resource(full_path, request.route, location->autoindex, index_file);
	}
	else if (method == "POST") {
		if (location->upload_path.empty()){
			std::string filename = extract_filename(request.route);
			std::string upload_path = location->upload_path;

			if(upload_path[upload_path.size() - 1] != '/')
				upload_path += "/";

			return save_resource(upload_path + filename, request.body);
		}
		else 
			return save_resource(full_path, request.body);
	}
	else
		return delete_resource(full_path);

	// This should never happen due to validation, but just in case
	// throw HttpCodeException(METHOD_NOT_ALLOWED, "Error: method not supported");
}


// ═══════════════════════════════════════════════════════════════════
// REQUEST PROCESSING
// ═══════════════════════════════════════════════════════════════════


void	Webserv::processClientRequest(size_t& idx) {
	const int client_fd = _poll_vector[idx].fd;
	ClientState& client = _client_map[client_fd];

	try {
		client._http_request = Parser::parseHttpRequest(client._request_buffer);

		const HttpRequest& request = client._http_request;

		// Validate body size against server config
		validateBodySize(request, client._server_config);

		// Find matching location
		LocationConfig* location = findLocation(request.route, client._server_config);
		
		if (location == NULL)
			throw ParseException("Error: Location is NULL");

		// Validate method is allowed
		if (!isAllowedMethod(request.method, location->allowed_methods))
			throw HttpCodeException(METHOD_NOT_ALLOWED, "Error: method not allowed");

		// Build filesystem path for CGI check
		std::string full_path = buildFullPath(request.route, location, client._server_config);

		// Handle CGI requests
		if (isCgiRequest(full_path, location->cgi_extension)) {
			CgiHandler cgi(request, full_path);
			int pipe_fd = cgi.executeCgi(client._cgi_pid, location->cgi_extension);

			client._cgi_pipe_fd = pipe_fd;

			_cgi_to_client[pipe_fd] = client_fd;

			addPollEvent(pipe_fd, POLLIN);
			return;
		}

		// Handle static requests
		client._response_buffer = handleStaticRequest(request, location, client._server_config);
		client._response_ready = true;
		setPollEvent(client_fd, POLLOUT);

	} catch (const PendingRequestException& e) {
		std::cout << "Client " << client_fd << ": " << e.what() << std::endl;
		return;
	} catch (const HttpCodeException& e) {
		std::cerr << e.what() << std::endl;
		client._response_buffer = e.httpResponse(client._server_config);
		client._response_ready = true;
		setPollEvent(client_fd, POLLOUT);
	}
}


// ═══════════════════════════════════════════════════════════════════
// EVENT HANDLERS
// ═══════════════════════════════════════════════════════════════════


void	Webserv::handleNewConnection(int socket_fd) {
	struct sockaddr_in address;
	socklen_t socklen = sizeof(address);

	int agent = accept(socket_fd, reinterpret_cast<sockaddr*>(&address), &socklen);

	if (agent == -1)
		throw SocketException("Error: accept() failed");

	Socket::setNonBlocking(agent);

	addPollEvent(agent, POLLIN);
	_client_map[agent] = ClientState();
	_client_map[agent]._server_config = getServerBySocketFd(socket_fd);
	std::cout << "New client connected: " << agent << std::endl;
}


void	Webserv::handleCgiEvent(size_t& idx) {
	int pipe_fd = _poll_vector[idx].fd;

	if (_cgi_to_client.find(pipe_fd) == _cgi_to_client.end()) {
		_poll_vector.erase(_poll_vector.begin() + idx);
		--idx;
		return;
	}

	int client_fd = _cgi_to_client[pipe_fd];

	if (_client_map.find(client_fd) == _client_map.end()) {
		_cgi_to_client.erase(pipe_fd);
		close(pipe_fd);
		_poll_vector.erase(_poll_vector.begin() + idx);
		--idx;
		return;
	}

	ClientState& client = _client_map[client_fd];

	try {
		ssize_t bytes_read = readFd(pipe_fd, client._response_buffer, PIPE);

		if (bytes_read > 0) {
			client._last_active = time(NULL);
			return;
		}

		if (bytes_read == 0) {
			client._response_buffer = CgiHandler::process_response(client._response_buffer);
			cleanupCgiProcess(client_fd);
			client._response_ready = true;
			setPollEvent(client_fd, POLLOUT);
			_poll_vector.erase(_poll_vector.begin() + idx);
			--idx;
		}
		else
			throw HttpCodeException(INTERNAL_ERROR, "Error: CGI pipe read error");

	} catch (const HttpCodeException& e) {
		std::cerr << e.what() << std::endl;
		client._response_buffer = e.httpResponse(client._server_config);
		cleanupCgiProcess(client_fd);
		client._response_ready = true;
		setPollEvent(client_fd, POLLOUT);
		_poll_vector.erase(_poll_vector.begin() + idx);
		--idx;
	}
}


void	Webserv::handleReceiveEvent(size_t& idx) {
	int client_fd = _poll_vector[idx].fd;
	ClientState& client = _client_map[client_fd];

	ssize_t bytes_read = readFd(client_fd, client._request_buffer, SOCKET);

	if (bytes_read <= 0)
		return disconnectClient(idx);

	client._last_active = time(NULL);

	if (client._request_buffer.find("\r\n\r\n") != std::string::npos)
		processClientRequest(idx);
}


void	Webserv::handleSendEvent(size_t& idx) {
	int client_fd = _poll_vector[idx].fd;
	ClientState& client = _client_map[client_fd];

	if (!client._response_ready)
		return;

	std::string& response = client._response_buffer;

	ssize_t bytes_sent = send(client_fd, response.c_str(), response.size(), 0);
	if (bytes_sent == -1) {
		disconnectClient(idx);
		return;
	}

	response.erase(0, bytes_sent);

	if (response.empty()) {
		resetClientInfo(client_fd);
		client._last_active = time(NULL);
		_poll_vector[idx].events = POLLIN;
	}
}


// ═══════════════════════════════════════════════════════════════════
// MAIN LOOP
// ═══════════════════════════════════════════════════════════════════


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
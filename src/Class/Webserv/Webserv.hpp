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
		 * 
		 * This function initializes and returns a `pollfd` structure configured
		 * with the given file descriptor and events to monitor.
		 * 
		 * @param fd_socket File descriptor of the socket.
		 * @param event Events to monitor (e.g., POLLIN, POLLOUT).
		 * @return Configured `pollfd` struct.
		 */
		static pollfd createPollfd(int fd_socket, short event);

		/**
		 * @brief Watches for poll events on all monitored file descriptors every 5 seconds.
		 * 
		 * This function continuously monitors the file descriptors in the poll vector
		 * for events. It blocks for up to 5 seconds waiting for events to occur.
		 * 
		 * @throws PollException If the `poll()` system call fails.
		 */
		void		watchPollEvents(void);

		/**
		 * @brief Adds a poll event for a given socket file descriptor.
		 * 
		 * This function adds a new `pollfd` structure to the poll vector to monitor
		 * events on the specified socket file descriptor.
		 * 
		 * @param sock_fd Socket file descriptor to monitor.
		 * @param event Events to monitor (e.g., POLLIN, POLLOUT).
		 */
		void		addPollEvent(int sock_fd, short event);

		/**
		 * @brief Handles a new incoming connection on the given socket file descriptor.
		 * 
		 * This function accepts a new client connection on the specified server socket
		 * and adds the client to the poll vector for monitoring.
		 * 
		 * @param socket_fd The server socket file descriptor on which the new connection was received.
		 * @throws SocketException If the `accept()` system call fails.
		 */
		void		handleNewConnection(int socket_fd);

		/**
		 * @brief Handles data reception from a client.
		 * 
		 * This function reads data from the client associated with the given index
		 * in the poll vector and updates the client's request buffer.
		 * 
		 * @param idx Index of the client in the poll vector.
		 */
		void		handleReceiveEvent(size_t& idx);
		
		/**
		 * @brief Handles CGI event for a client.
		 * 
		 * This function processes the output of a CGI script for the client
		 * associated with the given index in the poll vector.
		 * 
		 * @param idx Index of the client in the poll vector.
		 */
		void		handleCgiEvent(size_t& idx); // added

		/**
		 * @brief Handles sending data to a client.
		 * 
		 * This function sends the prepared HTTP response to the client associated
		 * with the given index in the poll vector.
		 * 
		 * @param idx Index of the client in the poll vector.
		 */
		void		handleSendEvent(size_t& idx);

		/**
		 * @brief Controls client connection timeouts and disconnects inactive clients.
		 * 
		 * This function checks the last activity time of all connected clients and
		 * disconnects those that have been inactive for too long.
		 */
		void		checkTimeout(void);

		/**
		 * @brief Resets the state information of a client after the response is sent.
		 * 
		 * This function clears the client's request and response buffers and resets
		 * its state to prepare for the next request.
		 * 
		 * @param socket_fd Client's socket file descriptor.
		 */
		void		resetClientInfo(int socket_fd);

		/**
		 * @brief Checks if a file descriptor belongs to a server socket.
		 * 
		 * This function determines whether the given file descriptor corresponds
		 * to one of the server sockets managed by the Webserv instance.
		 * 
		 * @param fd File descriptor to check.
		 * @return true if the file descriptor is a server socket.
		 * @return false otherwise.
		 */
		bool		isServerSocket(int fd) const;

		/**
		 * @brief Checks if a request is a CGI request based on the file extension.
		 * 
		 * This function determines whether the requested resource should be handled
		 * as a CGI request by comparing the file extension of the resource with the
		 * configured CGI extension.
		 * 
		 * @param full_path Full path of the requested resource.
		 * @param cgi_extension Pair containing the CGI extension and its handler.
		 * @return true if the request matches the CGI extension.
		 * @return false otherwise.
		 */
		bool		isCgiRequest(const std::string& full_path, const std::pair<std::string, std::string>& cgi_extension);

		/**
		 * @brief Cleans up resources associated with a CGI process for a client.
		 *
		 * This function terminates the CGI process associated with the client, closes
		 * the CGI pipe file descriptor, and removes the mapping between the CGI process
		 * and the client.
		 *
		 * @param client_fd The socket file descriptor of the client.
		 */
		void		cleanupCgiProcess(int client_fd);

		/**
		 * @brief Disconnects a client and cleans up associated resources.
		 *
		 * This function removes the client from the poll vector, closes the associated
		 * socket file descriptor, and clears the client's state from the internal map.
		 *
		 * @param idx The index of the client in the poll vector.
		 */
		void		disconnectClient(size_t& idx);

		/**
		 * @brief Processes an HTTP request received from a client.
		 *
		 * This function parses the HTTP request stored in the client's buffer, selects
		 * the appropriate server configuration, validates request limits (method and body size),
		 * resolves the location, handles redirections, CGI requests, or static resources,
		 * and prepares the response to be sent back to the client.
		 *
		 * @param idx The index in the poll vector that identifies the client.
		 *
		 * @throws PendingRequestException If the request is incomplete and needs more data.
		 * @throws HttpCodeException To generate an appropriate HTTP error response.
		 */
		void		processClientRequest(size_t& idx);
		void		setPollEvent(int fd, short event);

		// added
		void			handleRedirect(int client_fd, int code, const std::string& url);
		ServerConfig*	getServerByHost(int server_socket_fd, std::string& host_header);
		ServerConfig*	getClientServerConfig(const HttpRequest& request, ClientState& client);

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
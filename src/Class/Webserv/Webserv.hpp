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

	ServerConfig* _server_config; // added: pointer to the server the client came from

	ClientState(void) : _response_ready(false), _last_active(time(NULL)), _server_config(NULL) {}
};


/**
 * @brief Main server class managing sockets, polling, and client connections.
 */
class Webserv {
	private:
		std::vector<Socket*>		_server_sockets;
		std::vector<struct pollfd>	_poll_vector;
		std::map<int, ClientState>	_client_map;
		std::vector<ServerConfig>	_servers;

		/**
		 * @brief Creates a pollfd struct for a socket.
		 * @param fd_socket File descriptor.
		 * @param event Events to monitor.
		 * @return Configured pollfd struct.
		 */
		static pollfd createPollfd(int fd_socket, short event);

		void	watchPollEvents(void);
		void	addPollEvent(int sock_fd, short event);

		/**
		 * @brief Handles a new incoming connection on the given server socket.
		 * 
		 * @param socket_fd The file descriptor of the server socket.
		 */
		void	handleNewConnection(int socket_fd);
		/**
		 * @brief Handles incoming data on a client socket.
		 * 
		 * @param idx Index in the poll vector.
		 */
		void	handleReceiveEvent(size_t& idx);
		/**
		 * @brief Handles sending data to a client socket.
		 * 
		 * @param idx Index in the poll vector.
		 */
		void	handleSendEvent(size_t& idx);

		/**
		 * @brief Checks for client inactivity and disconnects timed-out clients.
		 */
		void	checkTimeout(void);
		void	resetClientInfo(int socket_fd);

		/**
		 * @brief Checks if a file descriptor belongs to a server socket.
		 * @param fd File descriptor to check.
		 * @return true if fd is a server socket.
		 */
		bool	isServerSocket(int fd) const;
		/**
		 * @brief Checks if the request should be handled by a CGI script.
		 * 
		 * @param request 
		 * @param full_path 
		 * @return true 
		 * @return false 
		 */
		bool	isCgiRequest(const HttpRequest& request, const std::string& full_path);

		void	disconnectClient(size_t& idx);
		/**
		 * @brief Processes the client's HTTP request.
		 * 
		 * @param idx Index in the poll vector.
		 */
		void	processClientRequest(size_t& idx);
		/**
		 * @brief Handles a static file request.
		 * 
		 * @param full_path The full path to the requested resource.
		 */
		void	handleStaticRequest(const std::string& full_path);

		// Disable copy (Rule of Three - Webserv manages dynamic resources)
		Webserv(const Webserv&);
		Webserv& operator=(const Webserv&);

		// added
		ServerConfig* getServerBySocketFd(int socket_fd);

	public:
		Webserv(void);
		Webserv(const char* av);
		Webserv& operator=(Webserv& other);
		~Webserv(void);

		void runServer(void);
		void addSocket(std::string& ip, int port);
};
#include <sys/socket.h> // socket()
#include <netinet/in.h> // struct sockaddr_in
#include <arpa/inet.h> // inet_addr()

#include <poll.h> // poll()

#include <unistd.h> // close()
#include <sstream> // std::ostringstream
#include <iostream> // std::containers()

#include <fcntl.h> // setsockopt()

#include <vector>
#include <map>

struct ClientState {
	std::string request_buffer;  // Stores the incoming request piece by piece
	std::string response_buffer; // Stores the final response to be sent
	bool        response_ready;  // Flag: true when we have finished processing and are ready to send

	ClientState() : response_ready(false) {}
};

void	set_nonblocking(int fd) {
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
		std::cerr << "fcntl failed" << std::endl;
}

pollfd	create_struct_pollfd(int fd_socket, short event) {
	pollfd pollfd;

	pollfd.fd = fd_socket;
	pollfd.events = event;
	pollfd.revents = 0;

	return pollfd;
}

int main(void) {
	int tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_socket == -1)
		std::cerr << "tcp_socket failed to create" << std::endl;

	set_nonblocking(tcp_socket);
	std::cout << "TCP Socket is: " << tcp_socket << std::endl;

	int opt = 1;
	if (setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
		std::cerr << "setsockopt failed" << std::endl;
		close(tcp_socket);
		return 1;
	}

	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(8080);
	address.sin_addr.s_addr = inet_addr("127.0.0.1");

	// bind() to bind the socket to IP/Port
	if (bind(tcp_socket, (const sockaddr *)&address, sizeof(address)) == -1) {
		std::cerr << "tcp_socket can't bind" << std::endl;
		close(tcp_socket);
	}

	// listen() to listen for connections
	if (listen(tcp_socket, 5) == -1) {
		std::cerr << "connection refused" << std::endl;
		close(tcp_socket);
	}

	std::vector<struct pollfd> poll_vector;
	std::map<int, ClientState> client_map;

	// Add server socket to poll
	struct pollfd tcp_poll = create_struct_pollfd(tcp_socket, POLLIN);
	poll_vector.push_back(tcp_poll);

	while (true) {
		int result = poll(poll_vector.data(), poll_vector.size(), 5000);

		if (result == -1) {
			std::cerr << "poll() failed" << std::endl;
			continue; 
		}

		if (result == 0) {
			std::cerr << "poll() timeout" << std::endl;
			continue; 
		}

		// Iterate through all sockets
		for (size_t i = 0; i < poll_vector.size(); ++i) {
			// Case 1: The Listening Socket (New Connection)
			if (poll_vector[i].fd == tcp_socket && (poll_vector[i].revents & POLLIN)) {
				socklen_t socklen = sizeof(address);
				int agent = accept(tcp_socket, (sockaddr *)&address, &socklen);
				if (agent == -1) {
					std::cerr << "agent -1" << std::endl;
					continue;
				}
				set_nonblocking(agent);
				
				if (agent >= 0) {
					// 1. Add new socket to poll vector
					poll_vector.push_back(create_struct_pollfd(agent, POLLIN)); // Start by listening
					
					// 2. Create new entry in the map (Birth of the client)
					client_map[agent] = ClientState();
					
					std::cout << "New client connected: " << agent << std::endl;
				}
			}
			// Case 2: Existing Client sending data (Reading)
			else if (poll_vector[i].revents & POLLIN) {
				int client_fd = poll_vector[i].fd;
				char buffer[1024];
				ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

				if (bytes_read <= 0) {
					// Error or disconnection: Close and clean up
					close(client_fd);
					client_map.erase(client_fd); // Remove memory from map
					poll_vector.erase(poll_vector.begin() + i);
					--i;
					std::cout << "Client disconnected" << std::endl;
				} else {
					buffer[bytes_read] = '\0';
					
					// Accumulate data in the map
					client_map[client_fd].request_buffer += buffer;

					// Simple check: Do we have the end of the HTTP headers?
					if (client_map[client_fd].request_buffer.find("\r\n\r\n") != std::string::npos) {
						std::cout << "Full request received from " << client_fd << std::endl;

						// Prepare the response (store it in the map, don't send yet!)
						std::string msg = "<h1>Hello from Webserv</h1>";
						std::ostringstream oss;
						oss << msg.size();
						client_map[client_fd].response_buffer =
							"HTTP/1.1 200 OK\r\n"
							"Content-Type: text/html\r\n"
							"Content-Length: " + oss.str() + "\r\n"
							"\r\n" + msg;

						client_map[client_fd].response_ready = true;

						// CRITICAL: Switch event to POLLOUT so poll() tells us when we can write
						poll_vector[i].events = POLLOUT;
					}
				}
			}
			// Case 3: Client ready to receive response (Writing)
			else if (poll_vector[i].revents & POLLOUT) {
				int client_fd = poll_vector[i].fd;
				
				// Double check if we actually have something to send
				if (client_map[client_fd].response_ready) {
					std::string &resp = client_map[client_fd].response_buffer;
					
					// Send data
					ssize_t bytes_sent = send(client_fd, resp.c_str(), resp.size(), 0);
					
					if (bytes_sent >= 0) {
						// For simplicity: assume full send. In real webserv, handle partial sends.
						std::cout << "Response sent to " << client_fd << std::endl;
						
						// Close connection (HTTP 1.0 style)
						close(client_fd);
						client_map.erase(client_fd); // Death of the client (clean memory)
						poll_vector.erase(poll_vector.begin() + i);
						--i;
					}
				}
			}
		}
	}
	close(tcp_socket);
}
#include <sys/socket.h> // socket()
#include <netinet/in.h> // struct sockaddr_in
#include <arpa/inet.h> // ???
#include <unistd.h> // close()
#include <sstream>
#include <iostream>

// Socket returns int to storage "fd"
// AF_INET = IPv4
// SOCK_STREAM = TCP
// 0 = Protocol chose automatically by last 2 params, in this case IPPROTO_TCP


int	main(void) {
	int tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	std::cout << "TCP Socket is: " << tcp_socket << std::endl;

	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(8080);
	address.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (bind(tcp_socket, (const sockaddr *)&address, sizeof(address)) == -1)
		std::cerr << "tcp_socket can't connect" << std::endl;

	if (listen(tcp_socket, 5) == -1)
		std::cerr << "connection refused" << std::endl;

	// accept() to accept connections and store client as new fd
	socklen_t socklen = sizeof(address);
	int agent = accept(tcp_socket, (sockaddr *)&address, &socklen);

	if (agent == -1)
		std::cerr << "agent failed to connect" << std::endl;

	// recv() to read in this context (agent)
	char	buffer[1024];
	ssize_t bytes_read = recv(agent, buffer, sizeof(buffer) - 1, 0);

	if (bytes_read > 0) {
		buffer[bytes_read] = '\0';
		std::cout << buffer << std::endl;
	}

	// send() returns n bytes that were sent successfully
	std::string msg = "<h1>Hello world</h1>";

	std::ostringstream oss;
	oss << msg.size();

	std::string	response =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: " + oss.str() + "\r\n"
		"\r\n" +
		msg;

	ssize_t bytes_sent = send(agent, response.c_str(), response.length(), 0);
	if (bytes_sent == -1)
		std::cerr << "Failed to send response" << std::endl;

	// close()
	close(agent);
	close(tcp_socket);
}

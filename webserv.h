#pragma once

// --- SOCKET ---
int     create_socket(int domain, int type, int protocol);
void	set_nonblocking(int& socket_fd);
void	set_socket_mode(int& socket_fd, int mode);
void    bind_socket(int& socket_fd, const char* ip, int port, int domain);
void    listen_socket(int& socket_fd, int backlog);
int		init_server_socket(void);

// --- FILE MANAGEMENT ---
std::string	get_file_content(const std::string& path);

// --- PARSE ---
const std::string get_mime_type(const std::string& extension);
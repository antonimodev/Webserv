# 🌐 Webserv

![Language](https://img.shields.io/badge/Language-C%2B%2B98-informational)
![Build](https://img.shields.io/badge/Build-make-success)
![Protocol](https://img.shields.io/badge/Protocol-HTTP%2F1.1-green)
![I/O](https://img.shields.io/badge/I%2FO-Non--Blocking-orange)
![Platform](https://img.shields.io/badge/Platform-Linux-lightgrey)

A lightweight, **non-blocking HTTP/1.1** server written in *C++98*.

## 🚀 Features

- **Non-Blocking I/O:** Built on `poll()` to handle multiple connections simultaneously without threads.
- **HTTP/1.1 Support:** Implements `GET`, `POST`, `DELETE`, and OPTIONS methods.
- **CGI Execution:** Runs scripts (Python, PHP, etc.) dynamically.
- **Virtual Hosting:** Supports multiple servers and ports in a single process.
- **Configuration Parsing:** Custom `.conf` file syntax inspired by NGINX.
- **File Management:** Robust file uploads and directory listings (Autoindex).
- **Resilient:** Handles large payloads (chunked transfer) and connection timeouts correctly.

## 🛠️ Installation & Usage

### 1. Build and Run
Clone the repository and compile the server using make:
```bash
> make
> ./webserv conf/test.conf
```
The server should now be running. By default, it listens on port 8080.

### 2. Testing the Server
You can access the server via your browser at `http://localhost:8080` or use curl for specific requests:

- Basic Request ➜ `curl -v http://localhost:8080`
- Upload file ➜ `curl -X POST -T image.png http://localhost:8082/uploads/`
- Delete file ➜ curl -X DELETE `http://localhost:8082/uploads/image.png`

## ⚙️ Configuration Example
The server is fully configurable via the .conf file.

```nginx
server {
    listen 8080;
    host 127.0.0.1;
    root ./static;
    client_max_body_size 10M;

    error_page 404 /ErrorPages/404.html;

    location / {
        allowed_methods GET POST;
        autoindex on;
    }

    location /cgi-bin {
        cgi_extension .py /usr/bin/python3;
    }
}
```

## 📂 Project Structure

```
.
├── src/
│   ├── Class/        # Core logic (Server, Parser, Sockets, CGI)
│   └── Exceptions/   # Custom HTTP exception handling
├── conf/             # Configuration files
├── static/           # Web resources (HTML, CSS, uploads)
└── Makefile
```

- `src/Class`: Core logic (Server, Parser, Sockets, CGI).
- `src/Exceptions`: Custom exception handling for HTTP error codes.
- `conf/`: Configuration files for testing different scenarios.
- `static/`: Web resources (HTML, CSS, Uploads folder).

## 👥 Authors

Developed by:

- [**antonimodev**](https://github.com/antonimodev)
- [**frmarian**](https://github.com/pacomariano28)
- [**jortiz-m**](https://github.com/jortiz-m)
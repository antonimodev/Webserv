# Struct sockaddr_in

```cpp
struct sockaddr_in {
    short            sin_family;   // e.g. AF_INET
    unsigned short   sin_port;     // e.g. htons(8080)
    struct in_addr   sin_addr;     // e.g. inet_addr("127.0.0.1")
    char             sin_zero[8];  // filled to make same size as struct sockaddr
};
```

# POLL()

```cpp
struct pollfd {
    int   fd;         // Socket to watch
    short events;     // Events we want read/write
    short revents;    // Events already done
};
```

## Event functions

- `POLLIN` There are data to read
- `POLLOUT` Can write without block
- `POLLERR` Error
- `POLLHUP` Client close connections

## How it works

```cpp
struct pollfd fds;
fds.fd = agent;
fds.events = POLLIN | POLLOUT;  // Both events
fds.revents = 0;

int ret = poll(fds, 1, 5000);

if (fds[0].revents & POLLIN)
    std::cout << "Readable" << std::endl;

if (fds[0].revents & POLLOUT)
    std::cout << "Writeable" << std::endl;

if (fds[0].revents & POLLERR)
    std::cout << "Socket error" << std::endl;

if (fds[0].revents & POLLHUP)
    std::cout << "Client closed connection" << std::endl;
```

TCP_SOCKET only needs POLLIN() because it watches all incoming connections

## Useful commands for testing

- Sequence of 5 connections

```bash
for i in 1 2 3 4 5; do curl http://127.0.0.1:8080 && echo " - Request $i done"; done
```

- Parallelism of 3 connections at same time

```bash
curl http://127.0.0.1:8080 & curl http://127.0.0.1:8080 & curl http://127.0.0.1:8080 &
```


## List of HTTP Response Codes

### HTTP Status Codes 1xx (Info)

- 100 Continue: The server has received the headers and the client should proceed.
- 101 Switching Protocols: The server agrees to switch protocols.
- 102 Processing: The server has received the request and is processing it.

### HTTP Status Codes 2xx (Success)

- 200 OK: The request was successful.
- 201 Created: The resource was successfully created.
- 204 No Content: The request was successful, but there is no content to return.

### HTTP Status Codes 3xx (Redirection)

- 301 Moved Permanently: The resource has been permanently moved to another URL.
- 302 Found: The resource is temporarily at another URL.
- 304 Not Modified: The resource has not been modified since the last request.

### HTTP Status Codes 4xx (Client Error)

- 400 Bad Request: The request has an incorrect format.
- 401 Unauthorized: Authentication is required.
- 403 Forbidden: Access to the resource is forbidden.
- 404 Not Found: The resource was not found.
- 429 Too Many Requests: The request limit has been exceeded.

### HTTP Status Codes 5xx (Server Error)

- 500 Internal Server Error: Internal server error.
- 502 Bad Gateway: The server received an invalid response from an upstream server.
- 503 Service Unavailable: The server cannot handle the request temporarily.
- 504 Gateway Timeout: The server did not receive a timely response from an upstream server.

## lvalue & rvalue

- lvalue: expression with memory address that can be addressed and modified.

- rvalue: temp value without memory address


## Common basic MIME types

### Text files:

- text/html: HyperText Markup Language (HTML)
- text/css: Cascading Style Sheets (CSS)
- text/plain: Plain text files
- text/javascript: JavaScript files

### Image files:

- image/jpeg: JPEG images
- image/png: PNG images
- image/gif: GIF images
- image/svg+xml: Scalable Vector Graphics (SVG)

### Application files:

- application/json: JSON data
- application/pdf: Portable Document Format (PDF)
- application/zip: ZIP archives
- application/octet-stream: A default for generic binary data

### Audio and video files:

- audio/mpeg: MP3 audio
- video/mp4: MP4 video


- EXCALIDRAW: https://excalidraw.com/#room=682b2c5c9c5835bb3fd2,5KFxse5OeORDs3z__RSN_A



### Parse Workflow

```cpp
tokens: ["server", "{", "listen", "8080", "host", "127.0.0.1", "}"]

i=2: token="listen"
  ↓ _serverHandlers.find("listen") → found
  ↓ it->second("8080", config)
  ↓ Calls handleListen("8080", config)
  ↓ config.listen_port = 8080

i=4: token="host"
  ↓ _serverHandlers.find("host") → found
  ↓ it->second("127.0.0.1", config)
  ↓ Calls handleHost("127.0.0.1", config)
  ↓ config.host = "127.0.0.1"
```

Request: GET /path/to/file
    ↓
Is there a matching location?
    ↓
YES → Use location config
NO  → Use default server config
    ↓
Is the path a directory?
    ↓
YES → Does index.html exist?
    ↓
    YES → Serve index.html
    NO  → Is autoindex = on?
        ↓
        YES → Show file list
        NO  → 403 Forbidden
    ↓
NO  → Serve file directly

### 🎯 **Action Plan**

1. **Routing**  
   _Without this, the configuration you parsed is not used. This is your critical responsibility._

2. **Chunked encoding**  
   _Necessary for large requests (uploads) and part of the subject._

3. **Validations**  
   _Allowed methods, body size, apply error pages._

4. **Redirections**  
   _Return 301/302 according to configuration._

---


# GITHUB COMMANDS

## Branch

```bash
git checkout -b branch-name      # Create and switch to new branch
git checkout branch-name         # Switch branch
git branch -d branch-name        # Delete local branch (-D to force)
git push origin --delete branch-name  # Delete remote branch
```

## Changes

```bash
git add .                        # Stage all changes
git commit -m "message"          # Commit changes
git push origin branch-name      # Push to remote
git pull                         # Pull from remote
```

## Merge

```bash
git checkout main                # Switch to target branch
git merge branch-name            # Merge branch into current
git push                         # Push merged changes
```

---

# Todos

- If location inside server Location is an image instead html, shows strange symbols

- If location does not exist, throws 404 not found, but... is early throw?

- Location->root (?)

- load_resource() -> Index_file (?)

- Check possible redundancies in HttpCodeException for every error code (404, 500, ...)

- May implement a "volatile bool" variable to replace while(true)
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


# Lista de Códigos de Respuesta HTTP

## Códigos de estado HTTP 1xx

1xx (Información)

- 100 Continue: El servidor ha recibido los encabezados y el cliente debe continuar.
- 101 Switching Protocols: El servidor acepta cambiar de protocolo.
- 102 Processing: El servidor ha recibido la solicitud y está procesándola.


## Códigos de estado HTTP 2xx

2xx (Éxito)

- 200 OK: La solicitud fue exitosa.
- 201 Created: El recurso fue creado exitosamente.
- 204 No Content: La solicitud fue exitosa, pero no hay contenido para devolver.


## HTTP Status Codes 3xx

3xx (Redirection)

- 301 Moved Permanently: The resource has been permanently moved to another URL.
- 302 Found: The resource is temporarily at another URL.
- 304 Not Modified: The resource has not been modified since the last request.


## HTTP Status Codes 4xx

4xx (Client Error)

- 400 Bad Request: The request has an incorrect format.
- 401 Unauthorized: Authentication is required.
- 403 Forbidden: Access to the resource is forbidden.
- 404 Not Found: The resource was not found.
- 429 Too Many Requests: The request limit has been exceeded.


## HTTP Status Codes 5xx

5xx (Server Error)

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

## Add changes to an existing Pull Request / Añadir cambios a un PR existente

**English:** Yes! You can add more changes to an existing Pull Request. Just make new commits to the same branch and push them:

**Español:** ¡Sí! Puedes añadir más cambios a un Pull Request existente. Solo haz nuevos commits en la misma rama y súbelos:

```bash
# 1. Make sure you're on the correct branch
git checkout your-branch-name

# 2. Make your changes to the files...

# 3. Stage the changes
git add .

# 4. Commit the new changes
git commit -m "Add additional changes to PR"

# 5. Push to the same branch (this updates the PR automatically!)
git push origin your-branch-name
```

**Note / Nota:** The PR will be automatically updated with your new commits. No need to create a new PR!
**El PR se actualizará automáticamente con tus nuevos commits. ¡No necesitas crear un nuevo PR!**

## Merge

```bash
git checkout main                # Switch to target branch
git merge branch-name            # Merge branch into current
git push                         # Push merged changes
```
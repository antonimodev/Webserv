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


## Códigos de estado HTTP 3xx

3xx (Redirección)

- 301 Moved Permanently: El recurso se movió permanentemente a otra URL.
- 302 Found: El recurso está temporalmente en otra URL.
- 304 Not Modified: El recurso no ha sido modificado desde la última solicitud.


## Códigos de estado HTTP 4xx

4xx (Error del Cliente)

- 400 Bad Request: La solicitud tiene un formato incorrecto.
- 401 Unauthorized: Se requiere autenticación.
- 403 Forbidden: El acceso al recurso está prohibido.
- 404 Not Found: El recurso no se encontró.
- 429 Too Many Requests: Se ha excedido el límite de solicitudes.


## Códigos de estado HTTP 5xx

5xx (Error del Servidor)

- 500 Internal Server Error: Error interno del servidor.
- 502 Bad Gateway: El servidor recibió una respuesta inválida de un servidor superior.
- 503 Service Unavailable: El servidor no puede manejar la solicitud temporalmente.
- 504 Gateway Timeout: El servidor no recibió una respuesta a tiempo de un servidor superior.



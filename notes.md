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
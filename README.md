# NetChat

A small socket-programming project in **C and C++** for **Linux**, using the POSIX sockets API. It has a multi-client TCP chat server, two clients (C and C++), and a UDP latency probe.

## Features
- **TCP chat server (`server.c`)**: a single-threaded server that uses `poll()` to handle many clients at once. It broadcasts each message to every other connected client and cleans up disconnected clients.
- **UDP echo port**: the same server echoes UDP packets on `port + 1`, which the latency probe uses.
- **C client (`client.c`)**: uses `poll()` to watch both the keyboard and the socket, so you can send and receive at the same time.
- **C++17 client (`client_cpp.cpp`)**: the same client written in an object-oriented style. A `Socket` class closes its file descriptor automatically (RAII), a `ChatClient` class holds the logic, and errors are reported with exceptions.
- **UDP latency probe (`udp_ping.c`)**: sends UDP packets and reports packets sent, packets received, loss, and min/avg/max round-trip time (using `CLOCK_MONOTONIC`).

## Build
Requires `gcc`, `g++` and `make` on Linux.

    make

## Run
Start the server in one terminal:

    ./server 5000        # TCP chat on port 5000, UDP echo on port 5001

Connect clients in two or more other terminals (mix the C and C++ clients):

    ./client 127.0.0.1 5000
    ./client_cpp 127.0.0.1 5000

Measure UDP round-trip latency:

    ./udp_ping 127.0.0.1 5001 50

## Concepts covered
- TCP vs UDP
- `socket`, `bind`, `listen`, `accept`, `connect`
- I/O multiplexing with `poll()`
- Connection lifecycle and resource cleanup
- RAII, classes and exception handling in C++17
- Measuring network latency and packet loss

## Possible extensions
- Usernames and a message-framing protocol
- A thread-pool version to compare with `poll()`
- Simulating packet loss with `tc netem`
- An RTP-style UDP stream with sequence numbers

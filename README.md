# NetChat: TCP Chat Server, UDP Latency Probe (C) and C++ Client (Linux)

Small socket-programming project in C using the POSIX sockets API.

- `server.c`: single-threaded, poll()-based multi-client TCP chat server (broadcasts each message to all other clients) plus a UDP echo port on port+1.
- `client.c`: chat client that multiplexes stdin and the socket with poll().
- `client_cpp.cpp`: C++17 version of the client using OOP and RAII (a `Socket` class that closes its fd automatically, a `ChatClient` class, exceptions for errors).
- `udp_ping.c`: UDP round-trip latency probe (sent/received/loss and min/avg/max RTT using CLOCK_MONOTONIC).

## Build and run
    make
    ./server 5000                  # TCP chat on 5000, UDP echo on 5001
    ./client 127.0.0.1 5000        # run in two or more terminals
    ./udp_ping 127.0.0.1 5001 50

## Concepts covered
TCP vs UDP, socket/bind/listen/accept/connect, I/O multiplexing with poll(), connection lifecycle and cleanup, non-blocking server loop design, latency measurement.

## Ideas to extend
Add usernames and a framing protocol, TLS, a thread-pool variant to compare with poll(), packet-loss simulation with `tc netem`, an RTP-style sequence-numbered UDP stream.

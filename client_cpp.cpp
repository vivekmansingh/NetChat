// C++17 chat client using RAII + OOP. Usage: ./client_cpp <host> <port>
#include <iostream>
#include <string>
#include <stdexcept>
#include <poll.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

class Socket {                       // RAII: closes fd automatically
    int fd_;
public:
    Socket() : fd_(::socket(AF_INET, SOCK_STREAM, 0)) { if (fd_ < 0) throw std::runtime_error("socket"); }
    Socket(const Socket &) = delete;
    Socket &operator=(const Socket &) = delete;
    ~Socket() { if (fd_ >= 0) ::close(fd_); }
    int fd() const { return fd_; }
    void connect(const std::string &host, int port) {
        sockaddr_in a{}; a.sin_family = AF_INET; a.sin_port = htons(port);
        if (inet_pton(AF_INET, host.c_str(), &a.sin_addr) != 1) throw std::runtime_error("bad address");
        if (::connect(fd_, reinterpret_cast<sockaddr *>(&a), sizeof a) < 0) throw std::runtime_error("connect");
    }
};

class ChatClient {
    Socket sock_;
public:
    ChatClient(const std::string &host, int port) { sock_.connect(host, port); }
    void run() {
        pollfd p[2] = {{0, POLLIN, 0}, {sock_.fd(), POLLIN, 0}};
        char buf[1024];
        while (poll(p, 2, -1) >= 0) {
            if (p[0].revents & POLLIN) { ssize_t r = read(0, buf, sizeof buf); if (r <= 0) break; (void)!write(sock_.fd(), buf, r); }
            if (p[1].revents & POLLIN) { ssize_t r = read(sock_.fd(), buf, sizeof buf); if (r <= 0) break; (void)!write(1, buf, r); }
        }
    }
};

int main(int argc, char **argv) {
    if (argc < 3) { std::cerr << "usage: " << argv[0] << " host port\n"; return 1; }
    try { ChatClient(argv[1], std::stoi(argv[2])).run(); }
    catch (const std::exception &e) { std::cerr << "error: " << e.what() << "\n"; return 1; }
}

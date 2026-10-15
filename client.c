/* Chat client: reads stdin, prints broadcasts. Usage: ./client <host> <port> */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(int argc, char **argv) {
    if (argc < 3) { fprintf(stderr, "usage: %s host port\n", argv[0]); return 1; }
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in a = {0};
    a.sin_family = AF_INET; a.sin_port = htons(atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &a.sin_addr) != 1) { fprintf(stderr, "bad address\n"); return 1; }
    if (connect(fd, (struct sockaddr *)&a, sizeof a) < 0) { perror("connect"); return 1; }
    struct pollfd p[2] = {{0, POLLIN, 0}, {fd, POLLIN, 0}};
    char b[1024];
    for (;;) {
        if (poll(p, 2, -1) < 0) break;
        if (p[0].revents & POLLIN) { ssize_t r = read(0, b, sizeof b); if (r <= 0) break; (void)!write(fd, b, r); }
        if (p[1].revents & POLLIN) { ssize_t r = read(fd, b, sizeof b); if (r <= 0) break; (void)!write(1, b, r); }
    }
    close(fd);
    return 0;
}

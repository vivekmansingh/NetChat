/* Multi-client TCP chat server (poll-based, single thread) + UDP echo port.
 * Build: make   Run: ./server 5000   (TCP chat on 5000, UDP echo on 5001) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CLIENTS 64
#define BUF 1024

static int make_socket(int type, int port) {
    int fd = socket(AF_INET, type, 0);
    if (fd < 0) { perror("socket"); exit(1); }
    int on = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);
    struct sockaddr_in a = {0};
    a.sin_family = AF_INET; a.sin_addr.s_addr = htonl(INADDR_ANY); a.sin_port = htons(port);
    if (bind(fd, (struct sockaddr *)&a, sizeof a) < 0) { perror("bind"); exit(1); }
    if (type == SOCK_STREAM && listen(fd, 16) < 0) { perror("listen"); exit(1); }
    return fd;
}

int main(int argc, char **argv) {
    int port = argc > 1 ? atoi(argv[1]) : 5000;
    int tcp = make_socket(SOCK_STREAM, port);
    int udp = make_socket(SOCK_DGRAM, port + 1);
    struct pollfd fds[MAX_CLIENTS + 2];
    int n = 2;
    fds[0] = (struct pollfd){tcp, POLLIN, 0};
    fds[1] = (struct pollfd){udp, POLLIN, 0};
    printf("TCP chat on %d, UDP echo on %d\n", port, port + 1);
    fflush(stdout);

    for (;;) {
        if (poll(fds, n, -1) < 0) { if (errno == EINTR) continue; perror("poll"); break; }
        if (fds[0].revents & POLLIN) {
            int c = accept(tcp, NULL, NULL);
            if (c >= 0) {
                if (n < MAX_CLIENTS + 2) { fds[n++] = (struct pollfd){c, POLLIN, 0}; dprintf(c, "* welcome, %d online\n", n - 2); }
                else { dprintf(c, "* server full\n"); close(c); }
            }
        }
        if (fds[1].revents & POLLIN) {           /* UDP echo for latency tests */
            char b[BUF]; struct sockaddr_in from; socklen_t fl = sizeof from;
            ssize_t r = recvfrom(udp, b, sizeof b, 0, (struct sockaddr *)&from, &fl);
            if (r > 0) sendto(udp, b, r, 0, (struct sockaddr *)&from, fl);
        }
        for (int i = 2; i < n; i++) {
            if (!(fds[i].revents & (POLLIN | POLLHUP | POLLERR))) continue;
            char b[BUF];
            ssize_t r = read(fds[i].fd, b, sizeof b);
            if (r <= 0) { close(fds[i].fd); fds[i] = fds[--n]; i--; continue; }
            for (int j = 2; j < n; j++)          /* broadcast to the others */
                if (j != i) (void)!write(fds[j].fd, b, r);
        }
    }
    return 0;
}

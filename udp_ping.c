/* UDP round-trip latency probe: ./udp_ping <host> <udp_port> [count] */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>

static double now_ms(void) { struct timespec t; clock_gettime(CLOCK_MONOTONIC, &t); return t.tv_sec * 1e3 + t.tv_nsec / 1e6; }

int main(int argc, char **argv) {
    if (argc < 3) { fprintf(stderr, "usage: %s host udp_port [count]\n", argv[0]); return 1; }
    int count = argc > 3 ? atoi(argv[3]) : 10, ok = 0;
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct timeval tv = {1, 0};
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof tv);
    struct sockaddr_in a = {0};
    a.sin_family = AF_INET; a.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &a.sin_addr);
    double sum = 0, min = 1e9, max = 0;
    for (int i = 0; i < count; i++) {
        char msg[32], rb[32]; snprintf(msg, sizeof msg, "ping %d", i);
        double t0 = now_ms();
        sendto(fd, msg, strlen(msg), 0, (struct sockaddr *)&a, sizeof a);
        if (recv(fd, rb, sizeof rb, 0) > 0) { double d = now_ms() - t0; sum += d; ok++; if (d < min) min = d; if (d > max) max = d; }
    }
    printf("sent=%d received=%d loss=%.0f%%", count, ok, 100.0 * (count - ok) / count);
    if (ok) printf(" rtt min/avg/max = %.3f/%.3f/%.3f ms", min, sum / ok, max);
    printf("\n");
    return 0;
}

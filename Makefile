CC=gcc
CFLAGS=-Wall -Wextra -O2
all: server client udp_ping client_cpp
%: %.c
	$(CC) $(CFLAGS) -o $@ $<
client_cpp: client_cpp.cpp
	g++ -std=c++17 -Wall -Wextra -O2 -o $@ $<
clean:
	rm -f server client udp_ping client_cpp

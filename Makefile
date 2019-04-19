CC = gcc
OPTS = -g -Wall -Werror -std=c99 -pthread -D_DEFAULT_SOURCE
INCS = $(wildcard *.h)

all: server client

server: server.o connect.o message.o lock.o server-threads.o
	$(CC) $(OPTS) $^ -o $@

client: client.o connect.o message.o client-threads.o
	$(CC) $(OPTS) $^ -o $@

%.o: %.c ${INCS}
	$(CC) $(OPTS) -c $< -o $@

.PHONY: clean
clean:
	rm server client *.o
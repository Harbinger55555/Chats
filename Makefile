CC = gcc
OPTS = -g -Wall -Werror -std=c99 -D_DEFAULT_SOURCE
CLIENT_OBJS := client.o connection.o message.o
INCS = $(wildcard *.h)

all: server client

server: server.o connection.o message.o
	$(CC) $(OPTS) $^ -o $@

client: client.o connection.o message.o
	$(CC) $(OPTS) $^ -o $@

%.o: %.c ${INCS}
	$(CC) $(OPTS) -c $< -o $@

.PHONY: clean
clean:
	rm main *.o
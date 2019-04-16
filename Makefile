CC = gcc
OPTS = -g -Wall -Werror -std=c99 -D_DEFAULT_SOURCE
SRCS = $(wildcard *.c)
INCS = $(wildcard *.h)
OBJS = $(SRCS:.cpp=.o)

all: main

main: ${OBJS}
	$(CC) $(OPTS) $^ -o $@

%.o: %.c ${INCS}
	$(CC) $(OPTS) -c $< -o $@

.PHONY: clean
clean:
	rm main *.o
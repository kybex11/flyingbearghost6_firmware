CC = gcc
CFLAGS = -Wall

all: source

source: main.c
	$(CC) $(CFLAGS) -o source main.c

run: source
	./source

clean:
	rm -f source
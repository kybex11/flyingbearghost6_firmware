CC = gcc
CFLAGS = -Wall

all: source

source: main.c
	-$(CC) $(CFLAGS) -o source main.c

run: source
	./source

output.bin: source
	-objcopy -O binary source output.bin

clean:
	-rm -f source output.bin
CC=gcc
CFLAGS=-Wall -O2
LIBS=-lssl -lcrypto -lpthread

all: server client

server: server.c
	$(CC) server.c -o server $(CFLAGS) $(LIBS)

client: client.c
	$(CC) client.c -o client $(CFLAGS) -lssl -lcrypto

clean:
	rm -f server client results.csv

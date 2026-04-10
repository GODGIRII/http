CC      = gcc
CFLAGS  = -Wall -Wextra -Iinclude
LDFLAGS = -lpthread

SRC     = src/common.c
SERVER  = src/server.c
CLIENT  = src/client.c

all: server client

server: $(SRC) $(SERVER)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

client: $(SRC) $(CLIENT)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f server client

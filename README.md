# tcp_chat

A simple TCP chat program written in C, built from scratch using POSIX sockets. The project consists of a multi-client server and a client that can connect to it and exchange messages in real time.

This is a learning project designed to demonstrate the fundamentals of socket programming — how two programs on the same machine (or across a network) establish a TCP connection and exchange data.

---

## How it works

The server listens on port 3000. When a client connects, the server spawns a new thread to handle that client, then immediately goes back to waiting for the next connection. This means multiple clients can be connected at the same time, each in their own thread.

The client connects to the server, reads a message from the terminal, sends it, and waits for the server to echo it back. This continues until either side sends `"bye"`, which closes the connection cleanly.

```
Client                        Server
──────                        ──────
socket()                      socket()
                              bind()     ← claim port 3000
                              listen()   ← wait for connections
connect() ── SYN ──────────►
           ◄─ SYN-ACK ───────  accept()  ← new thread per client
           ── ACK ──────────►

send(msg) ── "hello" ───────► recv()
            ◄── "hello" ──────  send()   ← echo back
recv()

send("bye") ── "bye" ───────► recv()
close()                        close()   ← connection ends
                              accept()  ← back to waiting
```

---

## Project structure

```
tcp_chat/
├── Makefile
├── README.md
├── include/
│   ├── common.h      — shared constants, headers, send_all() declaration
│   ├── server.h      — server function declarations
│   └── client.h      — client function declarations
└── src/
    ├── common.c      — send_all() implementation (used by both binaries)
    ├── server.c      — socket setup, accept loop, per-client thread handler
    └── client.c      — socket setup, connect, message input loop
```

Each file has a single responsibility. `common.c` holds anything shared between both sides. The `include/` folder keeps declarations separate from implementation so each binary only includes what it needs.

---

## Requirements

- GCC or Clang
- POSIX-compatible OS — Linux or macOS
- pthreads (included on Linux/macOS, no extra install needed)

---

## Building

```bash
# build both server and client binaries
make

# build only the server
make server

# build only the client
make client

# remove compiled binaries
make clean
```

This produces two executables in the project root: `server` and `client`.

---

## Running

Open two terminal windows.

**Terminal 1 — start the server:**
```bash
./server
```
```
server listening on port 3000
```

**Terminal 2 — start the client:**
```bash
./client
```
```
connected to 127.0.0.1 on port 3000
you:
```

Type a message in the client terminal and press Enter. The server echoes it back.

```
you: hello there
server: hello there
you: how are you
server: how are you
you: bye
server: bye
goodbye
```

The server terminal shows:
```
new connection from 127.0.0.1
client says: hello there
client says: how are you
client says: bye
closing connection
```

To test multiple clients, open a third terminal and run `./client` again while the first client is still connected. The server handles both simultaneously.

To stop the server, press `Ctrl+C`.

---

## Key concepts demonstrated

**Two sockets, one connection.** The server has two distinct file descriptors — `sockfd` (the listening socket, lives forever) and `client_sockfd` (one per connected client, closed when they leave). Beginners often confuse these. The listening socket is like the front door; the client socket is the table you're seated at.

**The accept loop.** The outer `while(1)` in the server calls `accept()` in a loop. Each iteration handles one new client. Without threading, this would be serial — client B waits while client A is being served. The thread-per-connection model fixes this.

**Partial sends and receives.** `recv()` can return fewer bytes than you asked for. The `send_all()` function in `common.c` loops until every byte is sent. This is a fundamental requirement of socket programming that this project handles correctly.

**Byte order.** Port numbers and IP addresses must be converted to network byte order before being placed in the address struct. `htons()` (host-to-network short) handles ports; `htonl()` handles IP addresses. The network always uses big-endian; your CPU may not.

**SO_REUSEADDR.** Set before `bind()` so you can restart the server immediately after stopping it, without hitting `Address already in use` from the OS's TIME_WAIT state.

**Thread safety.** The fd passed to each thread is heap-allocated (`malloc`) rather than sharing a stack variable. Without this, the main thread could overwrite the fd before the new thread reads it — a classic race condition.

---

## Known limitations

This is a learning project. A production server would additionally need:

- **Message framing.** TCP is a stream — there are no built-in message boundaries. Right now the protocol relies on fixed-size buffers and `strcmp`, which breaks if a message is split across two `recv()` calls. A real protocol prefixes each message with its length: `[4-byte length][data]`.
- **Thread pool.** Spawning a new OS thread per connection is expensive under high load. A thread pool with a work queue is more efficient.
- **Non-blocking I/O.** `epoll` (Linux) or `kqueue` (macOS) would allow a single thread to handle thousands of connections without spawning threads at all. This is how nginx works.
- **IPv6 support.** The server uses `AF_UNSPEC` via `getaddrinfo` which supports both IPv4 and IPv6, but the IP printing logic only handles IPv4.
- **TLS.** All messages are sent in plaintext. Wrapping the socket with OpenSSL would add encryption.

---

## What to read next

- `man 2 socket` — socket creation
- `man 2 bind` — binding to a port
- `man 2 listen` — backlog and passive mode
- `man 2 accept` — accepting connections
- `man 2 recv` / `man 2 send` — reading and writing
- `man 7 tcp` — TCP socket behavior overview
- Beej's Guide to Network Programming — beej.us/guide/bgnet

#ifndef CLIENT_H
#define CLIENT_H

int  create_client_socket(const char *host);
void run_client(int sockfd);

#endif

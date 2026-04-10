#ifndef SERVER_H
#define SERVER_H

int  create_server_socket(void);
void run_server(int sockfd);
void *handle_client(void *arg);

#endif

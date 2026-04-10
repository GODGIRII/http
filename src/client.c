#include "common.h"
#include "client.h"

int create_client_socket(const char *host) {
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host, PORT, &hints, &res) != 0) {
        perror("getaddrinfo");
        return -1;
    }

    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0) {
        perror("socket");
        freeaddrinfo(res);
        return -1;
    }

    if (connect(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("connect");
        close(sockfd);
        freeaddrinfo(res);
        return -1;
    }
    freeaddrinfo(res);

    printf("connected to %s on port %s\n", host, PORT);
    return sockfd;
}

void run_client(int sockfd) {
    char msg[BUFSIZE];
    char server_msg[BUFSIZE];

    while (1) {
        printf("you: ");
        fflush(stdout);

        if (fgets(msg, sizeof(msg), stdin) == NULL) break;

        msg[strcspn(msg, "\n")] = '\0';  // strip newline
        if (strlen(msg) == 0)   continue;

        if (send_all(sockfd, msg, strlen(msg)) < 0) {
            perror("send");
            break;
        }

        int n = recv(sockfd, server_msg, sizeof(server_msg) - 1, 0);
        if (n < 0) { perror("recv");                  break; }
        if (n == 0){ printf("server disconnected\n"); break; }

        server_msg[n] = '\0';
        printf("server: %s\n", server_msg);

        if (strcmp(msg, "bye") == 0) {
            printf("goodbye\n");
            break;
        }
    }

    close(sockfd);
}

int main(void) {
    int sockfd = create_client_socket("127.0.0.1");
    if (sockfd < 0) return 1;

    run_client(sockfd);
    return 0;
}

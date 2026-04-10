#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

int int main(int argc, char *argv[])
{
    struct sockaddr_in server_addr;

    server_addr.sin_family =AF_INET;
    server_addr.sin_port = htons(3000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int sockfd;
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        printf("couldn't create socket\n");
        return 1;
    }

    return EXIT_SUCCESS;
}

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int fd;
    int nread;
    
    if (argc < 4) {
        printf("Usage: %s <IP_address> <port> <bufsize>\n", argv[0]);
        exit(1);
    }
    
    int BUFSIZE = atoi(argv[3]);
    struct sockaddr_in servaddr;
    socklen_t SIZE = sizeof(struct sockaddr_in);
    
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creating");
        exit(1);
    }

    memset(&servaddr, 0, SIZE);
    servaddr.sin_family = AF_INET;

    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        perror("bad address");
        exit(1);
    }
    
    servaddr.sin_port = htons(atoi(argv[2]));

    if (connect(fd, (struct sockaddr *)&servaddr, SIZE) < 0) {
        perror("connect");
        exit(1);
    }

    char buf[BUFSIZE];

    write(1, "Input message to send\n", 22);
    while ((nread = read(0, buf, BUFSIZE)) > 0) {
        if (write(fd, buf, nread) < 0) {
            perror("write");
            exit(1);
        }
    }

    close(fd);
    exit(0);
}

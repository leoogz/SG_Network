#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAXDATASIZE 1000

int main(int argc, char *argv[])
{
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo;
    int rv;
    char s[INET_ADDRSTRLEN];

    if(argc != 3){
        fprint(stderr, "usage: client hostname portnum\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_sockatype = SOCK_STREAM;

    if((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo))!= 0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    if((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype,servinfo->ai_protocol)) == -1){
        perror("client: socket");
        return 2;
    }

    if(connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
        close(sockfd);
        perror("client : connect");
        return 2;
    }

    inet_ntop(servinfo->ai_family, &((struct_sockaddr_in*)servinfo->ai_addr)->sin_addr,s, sizeof s);
    printf("client: connecting to %s\n",s);

    freeaddrinfo(servinfo);

    strcpy(buf,"hello");
    if(send(sockfd, buf, strlen(buf), 0) == -1){
        perror("send");
        close(sockfd);
        exit(1);
    }

    buf[numbytes] = '\0';
    printf("client: received '%s'\n", buf);

    close(sockfd);
    return 0;
}

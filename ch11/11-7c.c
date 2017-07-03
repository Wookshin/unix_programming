#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PORTNUM 50045

int main(void){
    char buf[256];
    struct sockaddr_in sin;
    int sd;

    if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
	perror("socket");
	exit(1);
    }

    memset((char *)&sin, '\0', sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORTNUM);
    sin.sin_addr.s_addr = inet_addr("117.16.42.29");

    if(connect(sd, (struct sockaddr *)&sin, sizeof(sin))){
	perror("connect");
	exit(1);
    }

    printf("insert your sentences: ");
    scanf("%s",buf);

    if(send(sd, buf, sizeof(buf), 0) == -1){
	perror("send");
	exit(1);
    }

    if (recv(sd, buf, sizeof(buf), 0) == -1){
	perror("recv");
	exit(1);
    }
    close(sd);
    printf("From Server : %s\n", buf);

    return 0;
}

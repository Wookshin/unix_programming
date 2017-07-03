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
    struct sockaddr_in sin, cli;
    int sd, ns, clientlen = sizeof(cli), i=0;

    if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
	perror("socket");
	exit(1);
    }

    memset((char *)&sin, '\0', sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORTNUM);
    sin.sin_addr.s_addr = inet_addr("117.16.42.29");

    if(bind(sd, (struct sockaddr *)&sin, sizeof(sin))){
	perror("bind");
	exit(1);
    }

    if(listen(sd, 5) < 0 ){
	perror("listen");
	exit(1);
    }

    if ((ns = accept(sd, (struct sockaddr*)&cli, &clientlen)) == -1){
	perror("accept");
	exit(1);
    }
 
    if (recv(ns, buf, sizeof(buf), 0) == -1){
	perror("recv");
	exit(1);
    }

    while(buf[i] != '\0')
    {
	if(buf[i]>= 65 && buf[i]<=90)
	    buf[i] = buf[i]+32;
	else if(buf[i]>= 97 && buf[i] <= 122)
	    buf[i] = buf[i]-32;
	i++;
    }

    if (send(ns, buf, sizeof(buf), 0) == -1){
	perror("send");
	exit(1);
    }

    close(ns);
    close(sd);

    return 0;
}

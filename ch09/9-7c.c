#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>

int main(void){
    int pd, n;
    char inmsg[80];

    if ((pd = open("./HAN-FIFO", O_RDONLY)) == -1){
	perror("open");
	exit(1);
    }

    printf("Client ====\n");
    write(1, "From Server :", 13);
    
    n=read(pd, inmsg, 80);
    write(1, inmsg, n);
  
    if (n==-1) {
	perror("read");
	exit(1);
    }

    write(1, "\n", 1);
    close(pd);

    return 0;
}

    

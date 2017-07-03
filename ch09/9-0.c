#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(void){
    int fd[2];
    pid_t pid;
    char buf[257];
    int len;

    if(pipe(fd) == -1){
	perror("pipe");
	exit(1);
    }

    printf("%d %d\n",fd[0],fd[1]);

    write(fd[1], "shinwook\n", 10);
    read(fd[0], buf, 256);
    printf("%s",buf);
    read(fd[0], buf, 256);
    printf("%s",buf);
    return 0;
}
    

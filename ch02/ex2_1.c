#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(void){
    int  fd;
    close(0);
    mode_t mode;

    mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    fd = open("unix.txt", O_CREAT, mode);
    if (fd == -1) {
        perror("Create");
        exit(1);
    }
    printf("%d\n",fd);
    close(fd);

    return 0;
}


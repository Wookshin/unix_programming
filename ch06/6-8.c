#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int status;
    pid_t pid;
    switch (pid = fork()) {
	case -1 :
	  perror("fork");
	  exit(1);
	  break;
	case 0 :
	  printf("--> Child Process\n");
	  exit(7);
	  break;
	default :
	  wait(&status);
	  printf("--> Parent process\n");
	  printf("Status: %d, %x\n", status, status);
	  printf("Child process Exit Status:%d\n",status >> 8);
	  break;
    }
    return 0;
}

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    pid_t pid;

    switch (pid = fork()) {
	case -1 :
	  perror("fork");
	  exit(1);
	  break;
	case 0 :
	  printf("Child Process - My PID:%d, My Parent's PID:%d\n",(int)getpid(),(int)getppid());
	  break;
	default:
	  printf("Parent Process - My PID:%d, My Parent's PID:%d, My Child's PID:%d\n",
		(int)getpid(),(int)getppid(),(int)pid);
	  break;
    }

    printf("End of fork\n");
    return 0;
}

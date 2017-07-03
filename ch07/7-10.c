#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>

void handler(int signo){
    printf("Timer Invoked..");
}

int main(void){
    struct itimerval it;

    sigset(SIGALRM, handler);
    it.it_value.tv_sec = 3;
    it.it_value.tv_usec = 0;
    it.it_interval.tv_sec = 2;
    it.it_interval.tv_usec = 0;

    if (setitimer(ITIMER_REAL, &it, (struct itimerval *)NULL) == -1){
	perror("settimer");
	exit(1);
    }

    while(1){
	if(getitimer(ITIMER_REAL,&it) == -1) {
	  perror("getitimer");
	  exit(1);
	}
	printf("%d sec, %d msec.\n", (int)it.it_value.tv_sec, (int)it.it_value.tv_usec);
	sleep(1);
    }
    return 0;
}

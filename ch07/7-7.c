#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>

void handler(int signo){
    psignal(signo, "Received Signal:");
    sleep(10);
    printf("In Signal Handler, After Sleep\n");
}

int main(void){
    struct sigaction act;

    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGQUIT);
    act.sa_flags=SA_RESETHAND;
    act.sa_handler = handler;

    if(sigaction(SIGINT, &act, (struct sigaction *)NULL) <0){
	perror("sigaction");
	exit(1);
    }

    fprintf(stderr, "Input SIGINT: ");
    pause();
    fprintf(stderr, "After Signal Handler\n");

    return 0;
}

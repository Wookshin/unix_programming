#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>

void handler(int signo){
    psignal(signo, "Received Signal:");
}

int main(void){
    sigset_t set;

    sigset(SIGALRM, handler);

    sigfillset(&set);
    sigdelset(&set, SIGALRM);

    alarm(3);

    printf("Wait...\n");

    sigsuspend(&set);

    return 0;
}

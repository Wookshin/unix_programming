#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>

void handler(int signo){
    psignal(signo, "Received Signal");
}

int main(void){
    sigset(SIGALRM, handler);

    alarm(2);
    printf("Wait...\n");
    sleep(10);
    printf("finish...\n");
    alarm(2);
    sleep(10);
    printf("finish2...\n");
    return 0;
}

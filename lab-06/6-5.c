#include <unistd.h>
#include <signal.h>
#include <stdio.h>

void handler(int signo) {
    psignal(signo, "Received Signal");
}

int main(void){
    sigset(SIGALRM, handler);

    alarm(2);
    printf("Wait...\n");
    sleep(10);

    return 0;
}


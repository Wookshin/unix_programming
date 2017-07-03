#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>

void handler(int signo){
    printf("Signal Handler Signal Number : %d\n", signo);
    psignal(signo, "Received Signal");
}

int main(void){
    void (*hand)(int);

    hand = signal(SIGINT, handler);
    if (hand == SIG_ERR){
	perror("signal");
	exit(1);
    }

    printf("Wait 1st Ctrl+C... : SIGINT\n");
    pause();
    printf("After 1st Signal Handler\n");
    printf("Wait 2nd Ctrl+C... : SIGINT\n");
    pause();
    printf("After 2nd Signal Handelr\n");
    return 0;
}

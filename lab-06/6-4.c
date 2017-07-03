#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

void handler(int signo){
  printf("catch SIGINT...\n");
}

int main(void)
{
  int i;
  void (*hand)(int);
 
  hand = signal(SIGINT, handler);
  if (hand == SIG_ERR) {
    perror("signal");
    exit(1);
  }
 
  for (i=0; i<20; i++){
    printf("sleep 1 second...\n");
    sleep(1);
  }
  return 0;
}

#include <stdio.h>
#include <time.h>
#include <sys/times.h>
#include <unistd.h>
#include <stdlib.h>
#define CLK_TCK sysconf(_SC_CLK_TCK)

int main()
{
  int i;
  time_t t;
  struct tms mytms;
  clock_t t1, t2;

  if ((t1 = times(&mytms)) == -1){
	perror("times 1");
	exit(1);
  }


  for(i=0; i<1000000; i++)
	printf("%d\n",i);

  if ((t2 = times(&mytms)) == -1){
	perror("times 2");
	exit(1);
  }
  printf("Real time : %.3f sec\n",(double)(t2-t1)/CLK_TCK);
  printf("User time : %.3f sec\n",(double)mytms.tms_utime/CLK_TCK);
  printf("System time : %.3f sec\n",(double)mytms.tms_stime/CLK_TCK);
 
  return 0;
}


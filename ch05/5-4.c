#include <sys/types.h>
#include <sys/times.h>
#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>

int main(void) {
    int i;
    time_t t;
    struct tms mytms;
    clock_t t1,t2;

    if ((t1 = times(&mytms)) == -1) {
	perror("times 1");
	exit(1);
    }
 
    for(i=0; i < 999999; i++) 
	printf("time consume\n");

    if ((t2 = times(&mytms)) == -1) {
	perror("times 2");
	exit(1);
    }

    printf("Real time : %.lf sec\n", (double)(t2 - t1)/(clock_t)sysconf(3));
    printf("User time : %.lf sec\n", (double)mytms.tms_utime/(clock_t)sysconf(3));
    printf("System time : %.lf sec\n",(double)mytms.tms_stime / (clock_t)sysconf(3));
    return 0;
}

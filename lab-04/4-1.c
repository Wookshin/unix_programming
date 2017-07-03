#include <stdio.h>
#include <unistd.h>

int main (void) {
  printf("Max Arg : %ld\n", sysconf(_SC_ARG_MAX));
  printf("Max process per UID : %ld\n", sysconf(_SC_CHILD_MAX));
  printf("Clock Tick : %ld\n", sysconf(_SC_CLK_TCK));
  printf("Max Open File : %ld\n", sysconf(_SC_OPEN_MAX));
  printf("Version of POSIX.1 : %ld\n", sysconf(_SC_VERSION));

  return 0;
}

  
  

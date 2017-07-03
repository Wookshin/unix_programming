#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

int main(void){
  FILE *fp;
  int fd;
  char str[BUFSIZ];

  fd = open("unix.txt", O_RDONLY);
  if (fd == -1){
    perror("open");
    exit(1);
  }

  fp = fdopen(fd, "r");
  
  fgets(str, BUFSIZE, fp);
  printf("Read : %s\n", str);

  fclose(fp);

#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(void) {
    if (rmdir("programming") == -1) {
	perror("linux");
	exit(1);
    }
    return 0;
}

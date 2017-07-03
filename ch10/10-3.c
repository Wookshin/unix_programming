#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <string.h>

struct mymsgbuf {
    long mtype;
    char mtext[80];
};

int main(void){
    key_t key;
    int msgid;
  
    key = ftok("keyfild",1);
    msgid = msgget(key, IPC_CREAT|0644);
    if (msgid == -1){
	perror("msgget");
	exit(1);
    }

    printf("Before IPC_RMID\n");
    system("ipcs -q");
    msgctl(msgid, IPC_RMID, (struct msqid_ds *)NULL);
    printf("After IPC_RMID\n");
    system("ipcs -q");
    return 0;
}

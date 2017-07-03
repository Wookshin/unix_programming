#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PORTNUM 50045				// 사용할 port number

char* checkHeader(int sd);
void sendMsg(int sd, char* str, int size);
void recvMsg(int sd, char*local_buf, int size);

int main(int argc, char*argv[]){
    char buf[256], id[20], pwd[20];
    char c;
    struct sockaddr_in sin;
    int sd, i;
    FILE * fp; 
 
    if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){	// 소켓 파일 기술자 생성
	perror("socket");
	exit(1);
    }

    memset((char *)&sin, '\0', sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORTNUM);
    sin.sin_addr.s_addr = inet_addr("117.16.42.29");

    if(connect(sd, (struct sockaddr *)&sin, sizeof(sin))){	// 서버에 접속 요청
	perror("connect");
	exit(1);
    }

    if(strcmp(argv[1], "init") == 0)		// 명령어 옵션이 init일 경우
    {
      sendMsg(sd, "init", strlen("init")+1);		// 서버에 init을 보낸다.

      printf("User ID: ");			// 사용자로부터 ID와 Password를 입력받아 서버에 보낸다.
      scanf("%s",id);
      sendMsg(sd, id, strlen(id)+1);

      printf("User Password: ");
      scanf("%s", pwd);
      sendMsg(sd, pwd, strlen(pwd)+1);

      recvMsg(sd, buf, sizeof(buf));		// 서버로부터 응답을 받는다.
      printf("%s\n",buf);
      if(strcmp(buf, "ok") == 0){		// 서버로부터 ok를 받았을 경우 .header파일을 생성하여 사용자 ID를 저장한다.
        if((fp = fopen(".header","w")) == NULL){
	    perror("fopen");
	    exit(1);
        }
        fprintf(fp,"%s",id);
        fclose(fp);  	 
      }
    }

    else if(strcmp(argv[1], "login") == 0) 		// 명령어 옵션이 login일 경우
    {
      sendMsg(sd, "login", strlen("login")+1);		// 서버에 login를 보낸다.

      printf("User ID: ");		// 사용자로부터 ID 및 Password를 입력받아 서버에 보낸다.
      scanf("%s", id);
      sendMsg(sd, id, strlen(id)+1);

      printf("User Password: ");
      scanf("%s", pwd);
      sendMsg(sd, pwd, strlen(pwd)+1);

      recvMsg(sd, buf, sizeof(buf));		// 서버로부터 응답을 받는다.
      printf("%s\n",buf);
    }	

    else if(strcmp(argv[1], "push") == 0)		// 명령어 옵션이 push일 경우
    {
      sendMsg(sd, "push", strlen("push")+1);		// 서버에 push를 보낸다.

      if(strcmp(checkHeader(sd), "ok") == 0){		// 서버로부터 ok를 받았을 경우 
        if((fp = fopen(argv[2], "r")) == NULL){		
  	    perror("fopen");
	    exit(1);
        }

	sendMsg(sd, argv[2], strlen(argv[2])+1);		// 저장하려는 파일의 이름을 서버에 보낸다.

	i=0;
        while((c = fgetc(fp)) != EOF)		// 저장하려는 파일의 내용을 buf에 저장한다.
	    buf[i++] = c;
        buf[i] = EOF;
        sendMsg(sd, buf, sizeof(buf));		// buf에 저장한 파일의 내용을 서버에 보낸다.
	memset(buf, '\0', sizeof(buf));
	recvMsg(sd, buf, sizeof(buf));		// 서버로부터 응답을 받는다.
        printf("%s\n", buf);
        fclose(fp);
      }
      else
	printf("Denied Permission!\n");
    }

    else if(strcmp(argv[1], "show") == 0)		// 명령어 옵션이 show일 경우
    {
      sendMsg(sd, "show", strlen("show")+1);		// 서버에 show를 보낸다.

      if(strcmp(checkHeader(sd), "ok") == 0)		// 서버로부터 ok를 받았을 경우
      {	
	if((argc == 3) && (strcmp(argv[2], "-a") == 0))	// -a 옵션이 추가 되었다면 서버에 "all"을 아니라면 "none"을 보낸다.
            sendMsg(sd, "all", strlen("all")+1);
	else
            sendMsg(sd, "none", strlen("none")+1);
	recvMsg(sd, buf, sizeof(buf));		// 서버로부터 응답을 받는다.
        printf("%s\n", buf);
      }
      else
	printf("Denied Permission!\n");
    }

    else if(strcmp(argv[1], "pull") == 0)		// 명령어 옵션이 pull일 경우
    {
      sendMsg(sd, "pull", strlen("pull")+1);		// 서버에 pull를 보낸다.

      if(strcmp(checkHeader(sd), "ok") == 0)		// 서버로부터 ok를 받았을 경우
      {	 
  	if((fp = fopen(argv[2], "w")) == NULL){		// 가져올 파일의 이름으로 w모드와 함께 파일을 연다.
	    perror("fopen");
	    exit(1);
        }

        sendMsg(sd, argv[2], strlen(argv[2])+1);		// 가져올 파일의 이름을 서버에 보낸다.
	recvMsg(sd, buf, sizeof(buf));		// 서버로부터 가져올 파일의 내용을 받는다.
	
	i=0;			// 서버로부터 받은 파일의 내용을 새로운 파일에 쓴다.
	while(buf[i] != EOF)
	  fputc(buf[i++], fp);

 	printf("Success!\n");	
	fclose(fp);
      } 
      else
        printf("Denied Permission!\n");
    }

    else if(strcmp(argv[1], "delete") == 0)		// 명령어 옵션이 delete일 경우
    {
      sendMsg(sd, "delete", strlen("delete")+1);		// 서버에 delete를 보낸다.

      if(strcmp(checkHeader(sd), "ok") == 0)		// 서버로부터 ok를 받았을 경우
      {
        sendMsg(sd, argv[2], strlen(argv[2])+1);		// 삭제할 파일의 이름을 서버에 보낸다.
	recvMsg(sd, buf, sizeof(buf));		// 서버로부터 응답을 받는다.
        printf("%s\n", buf);
      }
      else
        printf("Denied Permission!\n");
    }
    else if(strcmp(argv[1], "man") == 0){		// 명령어 옵션이 man일 경우
	system("clear");		// 화면을 clear한 후 도움말 정보를 출력한다.
	printf("\n\n");
	printf("1.init \n");
	printf("  사용자 정보를 초기화합니다. 서버와의 통신에 필요한 .header파일을 생성합니다. .header파일에는 사용자의 ID가 저장됩니다. \n");
	printf("\n\n");
	printf("2.login \n");
	printf("  서버에 로그인합니다. 사용자의 ID와 Password가 필요로 됩니다. 로그인을 해야만 push, pull, show, delete 명령을 수행할 수 있습니다. init 후에 사용해야 합니다.\n");
	printf("\n\n");
	printf("3.push 파일명\n");
	printf("  사용자가 소유하고 있는 파일들 중 서버에 저장하기 원하는 파일을 서버에 저장합니다. 저장하려는 파일의 이름과 동일한 파일이 서버에 존재한다면 서버는 그 파일에 대하여 형상관리를 해줍니다.\n");
	printf("\n\n");
	printf("4.pull 파일명\n");
	printf("  서버가 관리하고 있는 파일들 중 가져오길 원하는 파일을 서버로부터 사용자에게로 가져옵니다. 형상관리된 파일들도 가져올 수 있습니다. show -a를 통해 형상관리하고 있는 모든 파일의 이름을 알 수 있습니다.\n ");
	printf("\n\n");
	printf("5.show [인수들]\n");
	printf("  서버가 관리하고 있는 사용자 파일들의 목록을 보여줍니다. -a 옵션 추가 시, 형상관리하고 있는 모든 파일들의 목록을 볼 수 있습니다.\n");
	printf("\n\n");
	printf("6.delete 파일명\n");
	printf("  서버가 관리하고 있는 파일들 중 삭제하기 원하는 파일을 삭제할 수 있습니다. 형상관리되고 있는 파일들도 삭제할 수 있습니다. show -a를 통해 형상관리하고 있는 모든 파일의 이름을 알 수 있습니다.\n");
	printf("\n\n");
    }
    close(sd);

    return 0;
}

char* checkHeader(int sd){
    FILE* fp;
    char buf[256];
    if((fp = fopen(".header", "r")) == NULL){		// .header파일의 내용(사용자 ID값)을 서버에 보낸다.
	perror("fopen");
	exit(1);
    }
    fscanf(fp,"%s",buf);
    sendMsg(sd, buf, strlen(buf)+1);

    memset(buf, '\0', sizeof(buf));	
    recvMsg(sd, buf, sizeof(buf));		// 서버로부터 "ok"를 받으면 "ok"를 아니라면 "Denied Permission"을 리턴한다.
    fclose(fp);

    if(strcmp(buf, "ok") == 0)
	return "ok";
    else
	return "Denied Permission!";
}

void sendMsg(int sd, char* str, int size)
{
    char buf[256];
    memset(buf, '\0', sizeof(buf));
    strncpy(buf, str, size);		// 서버에 size크기만큼의 str을 보낸다.
    if(send(sd, buf, size, 0) == -1){
	perror("send");
	exit(1);
    }
}

void recvMsg(int sd, char* local_buf, int size)
{
    memset(local_buf, '\0', size);
    if (recv(sd, local_buf, size, 0) == -1){		// 서버로부터 응답을 받는다.
	perror("recv");
	exit(1);
    }
}

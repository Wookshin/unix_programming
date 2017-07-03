#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define PORTNUM 50045

char* checkHeader(int sd);
void sendMsg(int sd, char* str, int size);
void recvMsg(int sd, char* local_buf, int size);

int main(void){
    char c;
    char buf[256],buf1[256],id[20], pwd[20], opt[50], str[20];
    char login_list[20][20];
    int cnt = 0, file_check = 0, ch=0, log_check = 0, id_check=0;
    struct sockaddr_in sin, cli;
    int val = 1;
    int sd, ns, clientlen = sizeof(cli), i=0, n, un;
    FILE* wfp;
    FILE* rfp;
    DIR* dp;
    struct dirent * dent;
    struct dirent * dent1;

    if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){		// 소켓 파일기술자 생성
	perror("socket");
	exit(1);
    }

    if (setsockopt(sd, SOL_SOCKET,
                         SO_REUSEADDR, (char *) &val, sizeof val) < 0) {		// 소켓 옵션 세팅
	perror("setsockopt");
	close(sd);
	return -1;
    }

    memset((char *)&sin, '\0', sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORTNUM);
    sin.sin_addr.s_addr = inet_addr("117.16.42.29");

    if(bind(sd, (struct sockaddr *)&sin, sizeof(sin))){		// 소켓 파일기술자를 지정된 IP 주소/포트번호와 결합
	perror("bind");
	exit(1);
    }

    if(listen(sd, 10) < 0 ){		// 사용자의 접속 요청 대기
	perror("listen");
	exit(1);
    }

    while(1){
      if ((ns = accept(sd, (struct sockaddr*)&cli, &clientlen)) == -1){		// 사용자의 접속 허용
  	  perror("accept");
	  exit(1);
      }

      recvMsg(ns, opt, sizeof(opt));		// 사용자로부터 opt를 받는다.

      if(strcmp(opt, "init") == 0)		// opt가 init일 경우
      {
        recvMsg(ns, id, sizeof(id));		// 사용자의 id를 받는다.
        recvMsg(ns, pwd, sizeof(pwd));		// 사용자의 password를 받는다.

	if((rfp = fopen("password.txt","r")) == NULL){		// password.txt를 오픈한다.
	    perror("fopen");
	    exit(1);
	}

	while((n = fscanf(rfp, "%s %s", buf, buf1)) != EOF)	// 사용자의 id와 password.txt의 정보가 일치하는지 확인한다.
	{	
	  if(strcmp(buf, id) == 0)
	  	id_check++;
	}

	if(id_check == 0)	// 사용자 정보가 없다면 password.txt에 정보를 추가한 후, 사용자의 디렉토리를 생성한다.
	{
	  if((wfp = fopen("password.txt","a")) == NULL){
	      perror("fopen");
	      exit(1);
	  }

	  if(mkdir(id,0755) == -1){
	      perror("mkdir");
	      exit(1);
	  }

	  fprintf(wfp,"%s %s\n",id, pwd);
	  sendMsg(ns, "ok", strlen("ok")+1);
	  fclose(wfp);
	}
	else
	  sendMsg(ns, "Already exists ID", strlen("Already exists ID")+1);

	id_check = 0;
	memset(buf,'\0',sizeof(buf));
	memset(buf1,'\0',sizeof(buf1));
	memset(id,'\0',sizeof(id));
	memset(pwd,'\0',sizeof(pwd));
	fclose(rfp);
      }
    	
      else if(strcmp(opt, "login") == 0)
      {
        recvMsg(ns, id, sizeof(id));		// 사용자의 id를 받는다.
        recvMsg(ns, pwd, sizeof(pwd));		// 사용자의 password를 받는다.

	if((rfp = fopen("password.txt","r")) == NULL){		// password.txt를 "r"모드로 연다.
	    perror("fopen");
	    exit(1);
	}

	while((n = fscanf(rfp, "%s %s", buf, buf1)) != EOF)	// 사용자의 id와 password.txt의 정보가 일치하는지 확인한다.
	{
	    printf("ID: %s  PWD:%s\n",buf, buf1);
	    if(strcmp(buf, id) == 0){
	 	id_check++;
	    }
	}

	if((id_check != 0) && (strcmp(buf1, pwd) == 0))	// 사용자의 id와 password가 일치하다면 login_list에 추가하고, 사용자에게 ok응답을 보낸다.
	{
	  sendMsg(ns, "ok", strlen("ok")+1);
	  strcpy(login_list[cnt++], id);
	}
	else
	  sendMsg(ns, "invalid information", strlen("invalid information")+1);
	
	id_check = 0;
	memset(id,'\0',sizeof(id));
	memset(pwd,'\0',sizeof(pwd));
	memset(buf,'\0',sizeof(buf));
	memset(buf1,'\0',sizeof(buf1));
	fclose(rfp);
      }

      else if(strcmp(opt, "push") == 0)
      {
        recvMsg(ns, id, sizeof(id));		// 사용자의 id를 받는다.
	
	for(i=0; i<cnt; i++){			// 사용자의 id가 로그인 중인지 확인한다.
	  if(strcmp(id, login_list[i]) == 0)
		log_check++;
	}

	if(log_check == 1)			// 로그인이 맞을 시
	{
	  sendMsg(ns, "ok", strlen("ok")+1);
          recvMsg(ns, buf1, sizeof(buf1));	// 사용자로부터 file name을 받는다..
	  
	  strcpy(buf, "./");
	  strcat(buf, id);
	  if((dp = opendir(buf)) == NULL){
	      perror("opendir");
	      exit(1);
	  }

	  while((dent = readdir(dp)))
	  {
	    if(strcmp(dent->d_name, buf1) == 0)
	    {					// 동일한 파일 이름이 존재한다면 형상관리를 한다.
	      memset(buf,'\0',sizeof(buf));
	      memset(buf1,'\0',sizeof(buf1));
	      file_check++;					

	      strcpy(buf, id);		// buf에는 "id/파일이름" 경로 저장
  	      strcat(buf, "/");
	      strcat(buf, dent->d_name);		

	      strcpy(buf1, ".");		// buf1에는 ".파일이름" 저장
	      strcat(buf1, dent->d_name);

	      ch=1;
	      rewinddir(dp);		// 디렉토리 offset 초기화
	      while((dent1 = readdir(dp))){
		  if(strncmp(dent1->d_name, buf1, strlen(buf1)) == 0)	// ".파일이름"과 동일한 파일의 개수를 샌다.
			ch++;
	      }
	      memset(buf1,'\0',sizeof(buf1));

	      strcpy(buf1, id);	// buf1에는 "id/.파일이름.넘버" 저장
  	      strcat(buf1, "/.");
	      strcat(buf1, dent->d_name);
	      sprintf(str, ".%d", ch);
	      strcat(buf1, str);
	      rename(buf, buf1);		// 기존 파일 이름을 ".파일이름.넘버"로 변경(형상관리)
	      memset(str,'\0',sizeof(str));
	      memset(buf1,'\0',sizeof(buf1));

              if((wfp = fopen(buf, "w")) == NULL){	// 최신 파일 내용으로 파일을 생성한다.
	          perror("fopen");
	          exit(1);
              }    
	      memset(buf,'\0',sizeof(buf));

              recvMsg(ns, buf, sizeof(buf));	// 사용자로부터 파일 내용을 받는다.
	      i=0;
              while(buf[i] != EOF)
                fputc(buf[i++],wfp);
	      memset(buf,'\0',sizeof(buf));

	      sprintf(buf, "#%d Configuration",ch);
	      sendMsg(ns, buf, strlen(buf)+1);
	      memset(buf,'\0',sizeof(buf));
	      ch = 0;
	      break;
	    }
	  }
	
	  if(file_check == 0)			// 형상관리 되는 파일이 없다면
 	  {
	    strcpy(buf, id);			
	    strcat(buf, "/");
	    strcat(buf, buf1);			// buf에는 "id/파일이름" 저장
            if((wfp = fopen(buf, "w")) == NULL){
	        perror("fopen");
	        exit(1);
            }    
	    memset(buf,'\0',sizeof(buf));

            recvMsg(ns, buf, sizeof(buf));	// 사용자로부터 파일 내용을 받는다.
	    i=0;		// 파일 내용을 "id/파일이름"에 쓴다.
            while(buf[i] != EOF)
              fputc(buf[i++],wfp);

	    sendMsg(ns, "Success!", strlen("Success!")+1);
	  }
	  fclose(wfp);
	  closedir(dp);
	}
        else					// 로그인을 하지 않았을 때(log_check == 0)
	  sendMsg(ns, "Denied Permission!", strlen("Denied Permission!")+1);

	log_check = 0;
	file_check = 0;
	memset(id,'\0',sizeof(id));
	memset(buf,'\0',sizeof(buf));
	memset(buf1,'\0',sizeof(buf1));
      }

      else if(strcmp(opt, "show") == 0)
      {
        recvMsg(ns, id, sizeof(id));		// 사용자의 id를 받는다.

	for(i=0; i<cnt; i++){			// 사용자의 id가 로그인 중인지 확인한다.
	  if(strcmp(id, login_list[i]) == 0)
		log_check++;
	}

	if(log_check == 1){		// 로그인이 되었을 경우
	  sendMsg(ns, "ok", strlen("ok")+1);
          recvMsg(ns, buf1, sizeof(buf1));	// 사용자로부터 show 옵션을 받아 buf1에 저장한다.

	  strcpy(buf, id);
	  if((dp = opendir(buf)) == NULL){
	      perror("opendir");
	      exit(1);
	  }
	  memset(buf,'\0',sizeof(buf));

	  if(strcmp(buf1, "all") == 0)		// show 옵션이 all 이라면
	  {
	    while((dent = readdir(dp))){	// ., ..을 제외한 모든 파일의 목록을 보여준다.
		if(strlen(dent->d_name) > 3){
  	          strcat(buf, dent->d_name);
	          strcat(buf, " ");
		}
	    }
	  }
	  else		// show 옵션이 none이라면
	  {
	    while((dent = readdir(dp))){
	      if(dent->d_name[0] != '.'){	// 숨겨진 파일들을 제외한 파일 목록을 보여준다.
  	        strcat(buf, dent->d_name);
	        strcat(buf, " ");
	      }
	    }
	  }
	  sendMsg(ns, buf, sizeof(buf));	// 파일의 목록을 사용자에게 보낸다.

	  memset(buf1,'\0',sizeof(buf1));
	  closedir(dp);
	}

	else
	  sendMsg(ns, "Denied Permission!", strlen("Denied Permission!")+1);

	log_check = 0;
	memset(id,'\0',sizeof(id));
	memset(buf,'\0',sizeof(buf));
      }
      else if(strcmp(opt, "pull") == 0)
      {
        recvMsg(ns, id, sizeof(id));		// 사용자의 id를 받는다.

	for(i=0; i<cnt; i++){			// 사용자의 id가 로그인 중인지 확인한다.
	  if(strcmp(id, login_list[i]) == 0)
		log_check++;
	}

	if(log_check == 1){		// 로그인이 되었을 경우
	  sendMsg(ns, "ok", strlen("ok")+1);
          recvMsg(ns, buf, sizeof(buf));	// 사용자로부터 파일이름을 받는다.

	  strcpy(buf1, id);		// "id/파일이름"을 buf1에 저장하고 파일이름으로 파일을 연다.
          strcat(buf1, "/");
	  strcat(buf1, buf);
          if((rfp = fopen(buf1, "r")) == NULL){
	      perror("fopen");
	      exit(1);
          }    
	  memset(buf, '\0', sizeof(buf));
	
	  i=0;		// 파일의 내용을 buf에 저장한다.			
	  while((c = fgetc(rfp)) != EOF)
		buf[i++] = c;
	  buf[i] = EOF;
	  sendMsg(ns, buf, strlen(buf)+1);	// 파일의 내용을 사용자에게 보낸다.

	  fclose(rfp);
    	}

	else
	  sendMsg(ns, "Denied Permission!", strlen("Denied Permission!")+1);

	log_check = 0;
	memset(id, '\0', sizeof(id));
	memset(buf, '\0', sizeof(buf));
	memset(buf1, '\0', sizeof(buf1));
	
      }
      else if(strcmp(opt, "delete") == 0)
      {
        recvMsg(ns, id, sizeof(id));		// 사용자의 id를 받는다.

	for(i=0; i<cnt; i++){			// 사용자의 id가 로그인 중인지 확인한다.
	  if(strcmp(id, login_list[i]) == 0)
		log_check++;
	}
	if(log_check == 1){		// 로그인이 되었다면
	  sendMsg(ns, "ok", strlen("ok")+1);
          recvMsg(ns, buf1, sizeof(buf1));	// 사용자로부터 파일이름을 받는다.

	  strcpy(buf, id);		// buf에 "id/파일이름"을 저장한다.
	  strcat(buf, "/");
	  strcat(buf, buf1);
	
	  un = unlink(buf);		// 파일 삭제
	  if(un == -1){
	      perror("unlink");
	      exit(1);
	  }

	  sendMsg(ns, "Delete OK!", strlen("Delete OK!")+1);
	}

	else
	  sendMsg(ns, "Denied Permission!", strlen("Denied Permission!")+1);

	log_check = 0;
	memset(id,'\0',sizeof(id));
	memset(buf,'\0',sizeof(buf));
	memset(buf1,'\0',sizeof(buf1));
      }

      else
      {
      }
      memset(opt,'\0',sizeof(opt));
    }

    close(ns);
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

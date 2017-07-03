#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>

#define ALL 1			// ALL일 경우 모두 출력
#define NONE 0			// NONE일 경우 "."으로 시작하는 파일 및 디렉토리를 제외하고 출력

void dir_print(DIR*dp, int flag);	// 디렉토리 내의 파일과 디렉토리를 표시
DIR* open_dir(char* path);		
void detail_print(DIR *dp, char*path, int flag);	// 디렉토리 내의 파일과 디렉토리의 상세정보를 표시
void mode_print(unsigned int mode);			// mode 출력
void dir_Sort(struct dirent* dents, int cnt);		// 파일과 디렉토리를 이름순으로 정렬


int main(int argc, char*argv[]){
    DIR*dp;

    if(argc == 1)		// 현재 디렉토리(옵션 없음)
    {
	dp = open_dir(".");
	dir_print(dp, NONE);
	closedir(dp);
    }
    else
    {
	if(argv[1][0] != '-')		// 다른 디렉토리
	{
	  if(argc == 2)		// 다른 디렉토리(옵션없음)
	  {
	    dp = open_dir(argv[1]);
  	    dir_print(dp, NONE);
	    closedir(dp);
	  }
	  else		// 다른 디렉토리(옵션존재)
	  {		
	    if(argc == 4 && ((argv[2][1] == 'l' && argv[3][1] == 'a') || (argv[2][1]=='a' && argv[3][1] == 'l'))) // a,l옵션
	    {
		dp = open_dir(argv[1]);
		detail_print(dp, argv[1], ALL);	
		closedir(dp);
	    }
	    else if(argv[2][1] == 'a')		// a 옵션
 	    {
		dp = open_dir(argv[1]);
		dir_print(dp,ALL);
	  	closedir(dp);
	    }
  	    else if(argv[2][1] == 'l')		// l 옵션
	    {
		dp = open_dir(argv[1]);
		detail_print(dp, argv[1], NONE);
		closedir(dp);
	    }
	    else				// 다른 옵션
		printf("Error syntax in option\n");
	  }
        }
	else			// 현재 디렉토리(옵션존재)
	{		
	    if(argc == 3 && ((argv[1][1] == 'l' && argv[2][1] == 'a') || (argv[1][1]=='a' && argv[2][1] == 'l'))) // a,l옵션
	    {
		dp = open_dir(".");
		detail_print(dp, ".", ALL);
		closedir(dp);
	    }
	    else if((argv[1][1] =='a' && argv[1][2] == 'l')||(argv[1][1]=='l'&&argv[1][2]=='a'))
	    {
		dp = open_dir(".");
		detail_print(dp, ".", ALL);
		closedir(dp);
	    }
	    else if(argv[1][1] == 'a')		// a 옵션
 	    {
		dp = open_dir(".");
		dir_print(dp,ALL);
		closedir(dp);
	    }
	    else if(argv[1][1] == 'l')		// l 옵션
	    {
		dp = open_dir(".");
		detail_print(dp, ".", NONE);
		closedir(dp);
	    }
	    else				// 다른 옵션
		printf("Error syntax in option\n");	  
	}
    }
    return 0;
}

void dir_print(DIR *dp, int flag){
    struct dirent dents[100];
    struct dirent * dent;
    int cnt=0,i;

    while((dent = readdir(dp)))		//명시된 디렉토리에서 파일과 디렉토리를 읽어 배열에 저장
    {
	dents[cnt].d_ino = dent->d_ino;
	dents[cnt].d_off = dent->d_off;
	dents[cnt].d_reclen = dent->d_reclen;
	strcpy(dents[cnt].d_name,dent->d_name);
	cnt++;
    }

    dir_Sort(dents, cnt);		// 배열에 저장된 파일과 디렉토리를 이름순으로 정렬

    for(i=0; i<cnt; i++)
    {
	if(flag == NONE)				//flag가 NONE일 경우 "."으로 시작하는 파일을 제외하고 출력
	{
  	  if(dents[i].d_name[0] == '.')		
	    continue;
	}
	printf("%s  ",dents[i].d_name);		// 파일과 디렉토리 이름 출력
    }
    printf("\n");
}

DIR* open_dir(char* path){
    DIR* dp;
    if((dp = opendir(path)) == NULL){		//path에 지정된 디렉토리를 얻어냄
	perror("opendir: current");
	exit(1);
    }
    return dp;
}

void detail_print(DIR *dp, char* path, int flag){
    struct dirent dents[100];
    struct dirent * dent;
    struct stat buf;
    struct passwd *pw;
    struct group *grp;
    struct tm *tm;
    char str[100];
    int cnt=0,i;
    int total_size=0;

    while((dent = readdir(dp)))			// 명시된 디렉토리 내의 파일과 디렉토리를 배열에 저장
    {
	dents[cnt].d_ino = dent->d_ino;
	dents[cnt].d_off = dent->d_off;
	dents[cnt].d_reclen = dent->d_reclen;
	strcpy(dents[cnt].d_name,dent->d_name);
	cnt++;
    }

    dir_Sort(dents, cnt);			// 배열에 저장된 파일과 디렉토리를 이름순으로 정렬

    for(i=0; i<cnt; i++)
    {
	if(flag == NONE)			// flag가 None으로 설정 시 "."으로 시작하는 파일과 디렉토리를 제외하고 출력
	{
	  if(dents[i].d_name[0] == '.')
	    continue;
	} 
	strcpy(str, path);
	strcat(str,"/");
	strcat(str,dents[i].d_name);
	stat(str, &buf);			//해당 파일 및 디렉토리의 struct stat을 얻어냄
	if((int)buf.st_size%4096 == 0)			// 파일 및 디렉토리의 사이즈가 4096으로 떨어지면 1024byte의 갯수를 total_size에 저장
	    total_size+=((int)buf.st_size/4096)*4;
	else
	    total_size+=((int)buf.st_size/4096)*4 + 4;  // 파일 및 디렉토리의 사이즈가 4096으로 떨어지지 않으면 +4를 추가(4096bytes단위로 할당되므로)
    }

    printf("합계 %d\n",total_size);

    for(i=0; i<cnt; i++)
    {
	if(flag == 0)
	{
	  if(dents[i].d_name[0] == '.')
	    continue;
	} 
	strcpy(str, path);
	strcat(str,"/");
	strcat(str,dents[i].d_name);
	stat(str, &buf);			//해당 파일 및 디렉토리의 struct stat을 얻어냄
	pw = getpwuid((uid_t)buf.st_uid);	//struct passwd를 얻어냄
	grp = getgrgid((gid_t)buf.st_gid);	//struct group을 얻어냄
	tm = localtime(&buf.st_mtime);		//struct tm을 얻어냄
	mode_print((unsigned int)buf.st_mode);	//mode를 문자로 변환하여 출력
	printf("%2d ",(unsigned int)buf.st_nlink);
	printf("%s ",pw->pw_name);
	printf("%10s ",grp->gr_name);
	printf("%5d ",(int)buf.st_size);
	printf("%d-%02d-%02d %02d:%02d ",tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min);	//시간 출력
	printf("%s\n",dents[i].d_name);
    }
}

void mode_print(unsigned int mode)
{
    int kind = mode & S_IFMT;
    int arr[9] = { S_IRUSR, S_IWUSR, S_IXUSR, 		//배열에 가능한 모드들을 저장
		   S_IRGRP, S_IWGRP, S_IXGRP,
		   S_IROTH, S_IWOTH, S_IXOTH }; 
    int i;

    switch(kind) {			//mode에 S_IFMT를 and시킨 후 switch를 통해 어떤 type인지 알아냄
	case S_IFDIR:
	  printf("d");
	  break;
	case S_IFREG:
	  printf("-");
	  break;
	case S_IFBLK:
	  printf("b");
	  break;
    }

    for(i=0; i<9; i++)			//User, Group, Other의 권한 출력
    {    
      if(i%3 == 0)
      { 
	if((mode&arr[i])!=0)
	  printf("r");
        else
	  printf("-");
      }
      if(i%3 == 1)
      { 
	if((mode&arr[i])!=0)
	  printf("w");
        else
	  printf("-");
      }
      if(i%3 == 2)
      { 
	if((mode&arr[i])!=0)
	  printf("x");
        else
	  printf("-");
      }
    }
    printf(". ");
}

void dir_Sort(struct dirent *dents, int cnt){
    struct dirent temp;
    int i,j,index;

    for(i=0; i<cnt-1; i++)		//배열에 저장된 파일 및 디렉토리들을 selection sort를 사용하여 이름순으로 정렬
    {
      for(j=i+1; j<cnt; j++)
      {
	index=0;
	while(dents[i].d_name[index] == dents[j].d_name[index])	 // 서로 동일하지 않은 문자의 인덱스를 찾아냄
		index++;
	if(dents[i].d_name[index] < dents[j].d_name[index])	// 동일하지 않은 문자 비교
		;
	else
	{						// i의 이름이 j의 이름보다 더 크다면 swap!
	  temp.d_ino = dents[j].d_ino;
	  temp.d_off = dents[j].d_off;
	  temp.d_reclen = dents[j].d_reclen;
	  strcpy(temp.d_name, dents[j].d_name);

	  dents[j].d_ino = dents[i].d_ino;
	  dents[j].d_off = dents[i].d_off;
	  dents[j].d_reclen = dents[i].d_reclen;
	  strcpy(dents[j].d_name,dents[i].d_name);

	  dents[i].d_ino = temp.d_ino;
	  dents[i].d_off = temp.d_off;
	  dents[i].d_reclen = temp.d_reclen;
	  strcpy(dents[i].d_name,temp.d_name);
	}
      }
    }
}

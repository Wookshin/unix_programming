#include <stdio.h>
#include <time.h>
#include <string.h>

char* changeDay(int d){
  switch(d){
	case 1:
	  return "월";
	case 2:
	  return "화";
	case 3:
	  return "수";
	case 4:
	  return "목";
	case 5:
	  return "금";
	case 6:
	  return "토";
	case 0:
	  return "일";
	default:
	  return "Error";
  }
}
int main(void){
  struct tm *tm;
  time_t t;
  char day[10];

  time(&t);
  tm = localtime(&t);
  strcpy(day, changeDay(tm->tm_wday));
  printf("%d. %02d. %d. (%s) %d:%d:%d KST\n",tm->tm_year+1990,tm->tm_mon+1, tm->tm_mday,day,tm->tm_hour,tm->tm_min,tm->tm_sec);

  return 0;
}

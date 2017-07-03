#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* const argv[])
{
  int i;
  for(i=1; i<argc; i++)
    {
	if(argv[i][0] == '-' && argv[i][1] == 'x'){
	  long num1, num2;
	  num1 = strtol(argv[i+1],NULL,16);
	  num2 = strtol(argv[i+2],NULL,16);
    	  printf("%d %X\n", num1+num2, num1+num2);
	  break;
	}
	else if(argv[i][0] == '-'){
	  printf("%s %s :invalid option\n Usage: 1-2 [-x] n1 n2\n",argv[0],argv[1]);
	  exit(1);
	}
	else{
	  int num1, num2;
	  num1 = atoi(argv[i]);
	  num2 = atoi(argv[i+1]);
    	  printf("%d %X\n", num1+num2, num1+num2);
	  break;
	}	
    }
}

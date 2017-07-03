#include <pwd.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(void){
  struct passwd *pw;
  pw = getpwent();
  while(pw!=NULL)
  {
    if(getuid() == (int)pw->pw_uid)
    {
      printf("pw_uid: %d\n", (int)(pw->pw_uid));
      printf("*pw_gecos: %s\n",pw->pw_gecos);
      printf("*pw_dir: %s\n", pw->pw_dir);
      printf("*pw_shell: %s\n", pw->pw_shell);
      break;
    }
    pw = getpwent();
  }

  return 0;
}

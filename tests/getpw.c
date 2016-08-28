#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <pwd.h>

void sigfunc(int signo);

int main()
{
  struct passwd *ptr;

  signal(SIGALRM, sigfunc);
  alarm(1);

  while(1) {
    /*
    if( (ptr = getpwnam("ricann")) == NULL) {
      printf("getpwnam error!\n");
      exit(1);
    }

    //printf("pw_name = %s\n", ptr->pw_name);

    if(strcmp(ptr->pw_name, "ricann") != 0) {
      printf("return value corrupted!, pw_name = %s\n", ptr->pw_name);
    }//*/
  }

  return 0;
}

void sigfunc(int signo)
{
  struct passwd *p;

  printf("in signal function!\n");
  if( (p = getpwnam("root")) == NULL) {
    printf("getpwnam error!\n");
    exit(1);
  }

  printf("pw_name = %s\n", p->pw_name);

  alarm(1);
}

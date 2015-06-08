#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void sigfunc(int signo);

int main()
{

  signal(SIGUSR1, sigfunc);
  printf("before pause!\n");
  pause();
  printf("after pause!\n");
}

void sigfunc(int signo)
{
  printf("Capture signal %d!\n", signo);
}

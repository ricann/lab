#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/wait.h>

void sigfunc(int signo);

int main()
{
  pid_t pid;

  if( (signal(SIGCHLD, sigfunc)) == SIG_ERR)
    perror("signal error");

  if( (pid = fork()) < 0) {
    perror("fork error");
  } else if(pid == 0) {
    sleep(2);
    _exit(0);
  }

  pause();
  exit(0);
}

void sigfunc(int signo)
{
  pid_t pid;
  int status;

  printf("SIGCLD received!\n");
  if( (signal(SIGCHLD, sigfunc)) == SIG_ERR)
    perror("signal error");

  if( (pid = wait(&status)) < 0 )
    perror("wait error");

  printf("pid = %d\n", pid);
}

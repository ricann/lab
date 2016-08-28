#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>

void sigfunc(int signo);

//sigjmp_buf jmpbuf;
jmp_buf jmpbuf;

int main()
{
  int ret;

  signal(SIGUSR1, sigfunc);

  //ret = sigsetjmp(jmpbuf, 0);
  ret = setjmp(jmpbuf);
  printf("ret = %d\n", ret);

  printf("before pause!\n");
  pause();
  printf("after pause!\n");
}

void sigfunc(int signo)
{
  printf("Capture signal %d!\n", signo);
  //siglongjmp(jmpbuf, 1);
  longjmp(jmpbuf, 1);
  exit(1);
}

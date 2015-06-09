#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>

void sig_int(int signo);
void sig_alrm(int signo);
unsigned int sleep2(unsigned int nsecs);

static jmp_buf env_alrm;

int main()
{
  unsigned int unslept;

  if( (signal(SIGINT, sig_int)) == SIG_ERR)
    perror("signal error");

  //raise(SIGINT);
  unslept = sleep2(1);

  printf("sleep2 returned: %u", unslept);
  return 0;
}

void sig_int(int signo)
{
  int i, j;
  volatile int k;

  printf("\nsig_int starting..\n");
  for(i=0; i<300000; i++) {
    for(j=0; j<4000; j++) {
      k += i*j;
    }
  }
  printf("\nsig_int ending..\n");
}

void sig_alrm(int signo)
{
  printf("\nsig_alrm starting..\n");
  longjmp(env_alrm, 1);
}

unsigned int sleep2(unsigned int nsecs)
{
  if( (signal(SIGALRM, sig_alrm)) == SIG_ERR)
    return nsecs;

  if( (setjmp(env_alrm)) == 0) {
    alarm(nsecs);
    pause();
  }

  return alarm(0);
}

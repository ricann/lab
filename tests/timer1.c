#include <stdio.h> 
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
 
void TimeInt2Obj(int imSecond,struct timeval *ptVal)
{
	ptVal->tv_sec=imSecond/1000;
	ptVal->tv_usec=(imSecond%1000)*1000; 
}
 
void SignHandler(int SignNo)
{
	printf("Clock %d\n", SignNo);
}
 
int main()
{
	struct itimerval tval; 

	signal(SIGALRM,SignHandler);
	TimeInt2Obj(10000,&tval.it_value);			// 设初始间隔为1毫秒，注意不要为0
	TimeInt2Obj(2000,&tval.it_interval);	// 设置以后的重复间隔为1500毫秒
	setitimer(ITIMER_REAL,&tval,NULL); 
	while(getchar()!=EOF); 
	return 0; 
}

#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
 
void SignHandler(int iSignNo)
{
	printf("Capture sign no:%d\n",iSignNo); 
	fflush(stdout);
}
 
int main()
{
	signal(SIGINT,SignHandler); 
	signal(SIGQUIT,SignHandler); 
	while(1) 
		sleep(1); 
	return 0; 
}

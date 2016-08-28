/*
练习与验证：
针对于先前的5种输入情况，给下面代码再添加一些代码，使之能够进行如下各种形式的响应：
1 、[CTRL+c] [CTRL+c]时，第1个信号处理阻塞第2个信号处理；
2 、[CTRL+c] [CTRL+c]时，第1个信号处理时，允许递规地第2个信号处理；
3 、[CTRL+c] [CTRL+/]时，第1个信号阻塞第2个信号处理；
4 、read不要因为信号处理而返回失败结果。
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

int g_iSeq=0;

void SignHandler(int iSignNo)
{
	int iSeq=g_iSeq++; 

	printf("%d Enter %s,signo:%d.\n",iSeq, __FUNCTION__, iSignNo); 
	sleep(3); 
	printf("%d Leave %s,signo:%d\n",iSeq, __FUNCTION__, iSignNo); 
}

void SignHandlerNew(int iSignNo,siginfo_t *pInfo,void *pReserved)
{
	int iSeq=g_iSeq++; 

	printf("%d Enter %s,signo:%d.\n",iSeq, __FUNCTION__, iSignNo); 
	sleep(3); 
	printf("%d Leave %s,signo:%d\n",iSeq, __FUNCTION__, iSignNo); 
}

int main()
{
	char szBuf[8]; 
	int iRet; 
	struct sigaction act; 

	memset(&act, 0, sizeof(act));

	act.sa_sigaction = SignHandlerNew; 
	act.sa_handler = SignHandler;
	act.sa_flags = SA_SIGINFO; 
	//act.sa_flags |= SA_NODEFER; 
	act.sa_flags = SA_RESTART; 

	sigemptyset(&act.sa_mask); 
	sigaddset(&act.sa_mask, SIGQUIT);

	sigaction(SIGINT,&act,NULL); 
	sigaction(SIGQUIT,&act,NULL); 

	do
	{ 
		iRet=read(STDIN_FILENO,szBuf,sizeof(szBuf)-1); 
		if(iRet<0)
		{ 
			perror("read fail."); 
			break; 
		} 
		szBuf[iRet]=0; 
		printf("Get: %s\n",szBuf); 
	}while(strcmp(szBuf,"quit\n")!=0); 

	return 0; 
}

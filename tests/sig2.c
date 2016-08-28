/*
程序运行时，针对于如下几种输入情况(要输入得快)，看输出结果：
1、  CTRL+c] [CTRL+c] [CTRL+c]
2、  [CTRL+c] [CTRL+/]
3、  hello [CTRL+/] [Enter]
4、  [CTRL+/] hello [Enter]
5、  hel [CTRL+/] lo[Enter]
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
 
int g_iSeq=0;
 
void SignHandler(int iSignNo)
{
	int iSeq=g_iSeq++; 
	printf("%d Enter SignHandler,signo:%d.\n",iSeq,iSignNo); 
	sleep(3); 
	printf("%d Leave SignHandler,signo:%d\n",iSeq,iSignNo); 
	fflush(stdout);
}

int main()
{
	char szBuf[8]; 
	int iRet; 
	signal(SIGINT,SignHandler); 
	signal(SIGQUIT,SignHandler); 
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

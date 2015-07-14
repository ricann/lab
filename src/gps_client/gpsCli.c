#include "gpsCli.h"

void gettime(char *timestr, int len)
{
	time_t inter;
	time(&inter);

	struct tm *timenow;
	timenow = localtime(&inter);

	strftime(timestr, len, "%Y%m%d%H%M%S", timenow);
	timestr[strlen(timestr)] = '\0';

	//debug
	//printf("timestr : %s\n", timestr);
}

int main(int argc, char const *argv[])
{
	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVPORT);
	servaddr.sin_addr.s_addr = inet_addr(SERVADDR);

	if(servaddr.sin_addr.s_addr == INADDR_NONE)
	{
		perror("Invalid IP address");
		exit(1);
	}

	int sockfd;
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket");
		exit(1);
	}
    
    char timestamp[20];
    int n;

    while(1)
    {
    	gettime(timestamp, 20);
        
        n = 0 ;
    	n = sendto(sockfd, timestamp, strlen(timestamp),
    	       0, (struct sockaddr*)&servaddr, sizeof(servaddr));

    	//debug
    	printf("n = %d timestamp :: %s\n", n, timestamp);

    	sleep(1);
    }
	return 0;
}
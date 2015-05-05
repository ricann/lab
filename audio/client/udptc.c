#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "udptc.h"

static char ip[32];
static int port;

int main(int argc, char *argv[])
{
	int ret;
	int sockfd;
	struct sockaddr_in saddr;

	//init ip and port
	if(argc == 3)
	{
		strncpy(ip, argv[1], strlen(argv[1]));
		port = atoi(argv[2]);
	}
	else
	{
		strncpy(ip, IP_DEFAULT, strlen(IP_DEFAULT));
		port = PORT_DEFAULT;
	}

	//init socket
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0)
	{
		perror("Socket error");
		exit(1);
	}
	bzero(&saddr, sizeof(struct sockaddr_in));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	if(inet_aton(ip, &saddr.sin_addr) < 0)
	{
		perror("IP error");
		exit(1);
	}

	ret = sendto(sockfd, TEST_STR, strlen(TEST_STR)+1, 0, 
		(struct sockaddr *)&saddr, sizeof(struct sockaddr_in));

	if(ret == -1)
	{
		perror("sendto error!");
		exit(1);
	}

	return 0;
}

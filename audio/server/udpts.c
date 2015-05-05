#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "udpts.h"

static char buf[512];

int main(int argc, char *argv[])
{
	int ret;
	int sockfd;
	socklen_t sin_size;
	struct sockaddr_in my_addr;
	struct sockaddr_in ac_addr;

	//init socket
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
	{
		perror("Socket error");
		exit(1);
	}
	bzero(&my_addr, sizeof(struct sockaddr_in));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(PORT_DEFAULT);
	my_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1)
	{
	    perror("bind");
	    exit(1);
	}

	while(1)
	{
		ret = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&ac_addr, &sin_size);

		printf("Received datagram from %s:\n",inet_ntoa(ac_addr.sin_addr));
		printf("%s\n", buf);
		if (ret == -1) 
		{
			perror ("recvfrom");
			exit(1);
		}
		//ret = sendto(sockfd, buf, 128, 0, (struct sockaddr *)&ac_addr, sin_size);
	}

	close(sockfd);

	return 0;
}

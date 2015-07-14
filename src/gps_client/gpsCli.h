#ifndef _GPSCLI_H_
#define _GPSCLI_H_ 

#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#ifndef SERVADDR
#define SERVADDR "10.103.240.123"
#endif

#ifndef SERVPORT
#define SERVPORT 6666
#endif


void gettime(char *timestr, int len);


#endif /*_GPSCLI_H_ */


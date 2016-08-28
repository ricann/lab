#include <stdio.h>
#include <pthread.h>

int main()
{
	pthread_cond_t cond = PTHREAD_COND_INITIALIZER; 
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 

	//printf("PTHREAD_COND_INITIALIZER = %d\n", PTHREAD_COND_INITIALIZER);
	//printf("PTHEAD_MUTEX_INITIALIZER = %d\n", PTHEAD_MUTEX_INITIALIZER);

	return 0;
}

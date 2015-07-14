#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void * thread(void* c)
{
	char ch = *(char*)c;
	sleep(1);
	putchar(ch);
}

int main(int argc, char const *argv[])
{
	pthread_t id[7];
	int i, ret;
	char str[] = "abcdefg";

	for(i = 0; i < 7; ++i)
	{
		ret = pthread_create(&id[i], NULL, thread, &str[i]);
		if(0 != ret)
			i--;
	}

	for(i = 0; i < 7; ++i)
	{
		pthread_join(id[i], NULL);
		printf("\n");
	}

	return 0;
}
#include <stdio.h>
#include <string.h>

int main()
{
	unsigned char	buf1[32] = {0};
	unsigned char	buf2[32] = "test";

	printf("sizeof(\"test\") = %d\n", sizeof("test"));

	memcpy(buf1, "test", sizeof("test"));

	printf("buf1 = %s, strlen(buf1) = %d\n", buf1, strlen(buf1));

	return 0;
}

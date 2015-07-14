#include <stdio.h>
#include <unistd.h>
//#include <iostream>
#include <time.h>

//using namespace std;

int foo(int a, int b)
{
	if(a) return b;
	return -1;
}

int main(int argc, char const *argv[])
{
	int x = '0', y = '1', u = '2', v = '3';
	//printf("%c\n", foo(foo(x, y), foo(u, v)));
	printf("%c\n", foo(x, y));
	/*
	srand((unsigned)time(NULL));
	int MAXNUM = 100;
    int i;

    for (i = 0; i < 10; ++i)
    {
    	//cout << rand()%MAXNUM <<" ";
    	printf("%d ", rand()%MAXNUM);
    }
    //cout<<endl;
    printf("\n");
    */

    /*
    //int (*n)[10];
    char (*n)[10];
    printf("sizeof(n) = %d \n", sizeof(n));
    int i;
    for(i = 0; i < 10; i++)
    	(*n)[i] = "a";
	*/
    /*
	int i = 1; int j = i++;
	printf("i = %d, j = %d\n", i,j);
	if((i++==++j)&&(++i == j)) 
		{i+=j;
			printf("asd\n");}
	printf("%d\n", i);
	*/

	return 0;
}
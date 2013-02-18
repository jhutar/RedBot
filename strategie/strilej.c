#include <stdlib.h>
#include <time.h>
#include <stdio.h>

int main()
{
	srandom(time(NULL));
	printf("m %d %d\n", (random()) % 14, (random())%14);
	return 0;
}
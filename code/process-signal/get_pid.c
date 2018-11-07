#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
	printf("Current process ID is %d\n", getpid());
	return 0;
}


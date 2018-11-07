#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
	pid_t pid;
	pid = fork();
	if (pid ==0)
	{
		printf("child process\n");
		exit(0);
	}
	printf("pid:%d\n", pid);
	return 0;
}

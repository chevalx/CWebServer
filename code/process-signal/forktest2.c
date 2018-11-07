#include <stdio.h>
#include <unistd.h>

int main()
{
	pid_t pid;
	pid = fork();
	if (pid ==0)
		printf("child process\n");
	printf("pid:%d\n", pid);
	return 0;
}

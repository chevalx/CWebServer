#include <unistd.h>
#include <stdio.h>

int main()
{
	pid_t pid;
	pid = fork();
	printf("pid: %d\n", pid);
	return 0;
}

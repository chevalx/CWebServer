/*signals.c
 * catch several signals and print their numbers*/

#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

void sigroutine(int number)
{
	switch (number)
	{
		case SIGHUP:
			printf("Capture SIGHUP signal, the signal number is %d\n", number);
			break;
		case SIGINT:
			printf("Capture SIGINT signal, the signal number is %d\n", number);
			break;
		case SIGQUIT:
			printf("Capture SIGQUIT signal, the signal number is %d\n", number);
			break;
		case SIGTSTP:
			printf("Capture SIGTSTP signal, the signal number is %d\n", number);
			break;
	}
	
	return;
}

int main(void)
{
	printf("process ID is %d\n", getpid());
	if (signal(SIGHUP, sigroutine)==SIG_ERR)
		printf("Couldn't register signal handler for SIGHUP!\n");
	if (signal(SIGINT, sigroutine)==SIG_ERR)
		printf("Couldn't register signal handler for SIGINT!\n");
	if (signal(SIGQUIT, sigroutine)==SIG_ERR)
		printf("Couldn't register signal handler for SIGQUIT!\n");
	if (signal(SIGTSTP, sigroutine)==SIG_ERR)
		printf("Couldn't register signal handler for SIGTSTP!\n");
	while(1)
		sleep(1);
	return 0;
}

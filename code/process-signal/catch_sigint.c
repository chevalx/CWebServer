/*catch_sigint.c
	catch SIGINT signal */

#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void SignHandler(int iSigNum)
{
	printf("Capture signal number:%d\n", iSigNum);
}

int main(void)
{
	signal(SIGINT,SignHandler);
	while (1)
		sleep(1);
	return 0;
}

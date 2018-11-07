/*default_sigint.c
	set SIGINT signal action to default*/

#include <signal.h>
#include <stdio.h>
#include <unistd.h>

int main(void)
{
	signal(SIGINT,SIG_DFL);
	while (1)
		sleep(1);
	return 0;
}

/* ftok_test.c 
 * create a key by ftok and print it*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define PATHNAME "/home/lijk0630/netprogramming/code/ipc"

int main()
{
	key_t key;
	
	key = ftok(PATHNAME, 'a');
	if (key == -1)
	{
		fprintf(stderr, "Create Key Error:%s\n", strerror(errno));
		exit(1);
	}
	printf("key = 0x%lx\n", (long unsigned int)key);
	exit(0);
}

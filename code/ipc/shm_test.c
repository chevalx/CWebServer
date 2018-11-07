/* shm_test.c - shared memory test program
 * (1) The parent process writes the input string into a block of shared memory;
 * (2) The child process reads the content from the shared memory and prints it;
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define PATHNAME "/home/lijk0630/netprogramming/code/ipc"

int main(int argc, char **argv)
{
	int shmid;
	char *p_addr, *c_addr;
	key_t key;
	pid_t child_id;
	
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <string>\n", argv[0]);
		exit(1);
	}
	
	key = ftok(PATHNAME, 'a');
	if (key == -1)
	{
		perror("ftok error");
		exit(1);
	}
	
	shmid = shmget(key, 1024, IPC_CREAT);
	if (shmid == -1)
	{
		perror("Create Shared Memory Error");
		exit(1);
	}
	printf("The ID of shared memory is:%d\n", shmid);

	child_id = fork();
	if (child_id > 0)
	{
		/*parent process writes the input argument into shared memory*/
		p_addr = shmat(shmid, 0, 0);
		memset(p_addr, 0, 1024);
		strncpy(p_addr, argv[1], strlen(argv[1]));
		if (shmdt(p_addr) == -1)
		{
			perror("detach error");
			exit(1);
		}
		sleep(1);
	}
	else if (child_id == 0)
	{
		/*child process reads the content from shared memory and prints it*/
		//sleep(1);
		c_addr = shmat(shmid, 0, 0);
		printf("Child process gets: %s\n", c_addr);
		if (shmdt(c_addr) == -1)
		{
			perror("detach error");
			exit(1);
		}
		exit(0);
	}
	else
	{
		perror("fork error");
		exit(1);
	}
	wait(NULL);
	exit(0);
}

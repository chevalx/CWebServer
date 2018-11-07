/* server_oob.c
 * receive normal or out-of-band tcp packets from client and print them */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>

//server port number
#define MYPORT 4000
//socket's listening queue length
#define BACKLOG 10

//new sockt descriptor returned by accept function
int new_fd;

//function defined for SIGURG signal
void sig_urg(int signo)
{
	int n;
	char buf[100];
	printf("SIGURG received\n");
	bzero(buf, sizeof(buf));
	n = recv(new_fd, buf, sizeof(buf)-1, MSG_OOB);
	if (n < 0)
		printf("urg recv error: errno=%d\n", errno);
	printf("recv %d OOB bytes: %s\n", n, buf);
}

int main(int argc, char *argv[])
{
	int sock_fd;
	struct sockaddr_in my_addr;
	struct sockaddr_in their_addr;
	int sin_size, n;
	char buf[100];
	
	//create socket
	if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket");
		return 1;
	}
	
	//int oob_inline = 1;
	//setsockopt(sock_fd, SOL_SOCKET, SO_OOBINLINE, &oob_inline, sizeof(oob_inline));
	
	bzero(&my_addr, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(MYPORT);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	
	//bind socket to my_addr
	if(bind(sock_fd, (struct sockaddr*)&my_addr, sizeof(struct sockaddr)) == -1)
	{
		perror("bind");
		close(sock_fd);
		return 1;
	}
	
	//listen on socket
	if(listen(sock_fd, BACKLOG) == -1)
	{
		perror("listen");
		close(sock_fd);
		return 1;
	}
	
	//define function routine for SIGURG
	void *old_sig_urg_handle;
	if((old_sig_urg_handle = signal(SIGURG, sig_urg)) == SIG_ERR)
	{
		perror("signal");
		close(sock_fd);
		return 1;
	}
	
	while(1)
	{
		sin_size = sizeof(struct sockaddr_in);
		if((new_fd = accept(sock_fd, (struct sockaddr*)&their_addr, &sin_size)) == -1)
		{
			perror("accept");
			continue;
		}
		
		//set owner for new_fd
		if (fcntl(new_fd, F_SETOWN, getpid()) == -1)
		{
			perror("fcntl");
			close(new_fd);
			close(sock_fd);
			exit(1);
		}
		
		printf("server: got connection from [%s]\n", inet_ntoa(their_addr.sin_addr));
		while(1)
		{
			bzero(buf, sizeof(buf));
			if((n = recv(new_fd, buf, sizeof(buf), 0)) == 0)
			{
				printf("recv EOF\n");
				break;
			}
			printf("Recv: %d bytes: %s\n", n, buf);
		}
		close(new_fd);
	}
	
	//restore the old handler for SIGURG signal
	signal(SIGURG, old_sig_urg_handle);
	
	close(sock_fd);
	return 0;
}

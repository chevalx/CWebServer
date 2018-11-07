/*oobmark_server.c
 *accept client's connection,send normal and OOB data to client*/
 
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <strings.h>

#define PORT 3000

int main(int argc,char **argv)
{
	int sockfd,new_fd;
	struct sockaddr_in srvaddr;

	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)
	{
		printf("socket error\n");
		exit(1);
	}

	bzero(&srvaddr,sizeof(srvaddr));
	srvaddr.sin_family=AF_INET;
	srvaddr.sin_port=htons(PORT);
	srvaddr.sin_addr.s_addr=htonl(INADDR_ANY);

	if(bind(sockfd,(struct sockaddr *)&srvaddr,sizeof(struct sockaddr))==-1)
	{
		printf("bind error\n");
		close(sockfd);
		exit(1);
	}

	if(listen(sockfd,5)==-1)
	{
		printf("listen error\n");
		close(sockfd);
		exit(1);
	}

	for(;;)
	{
		new_fd=accept(sockfd,NULL,NULL);
		if(new_fd==-1)
		{
			printf("accept error\n");
			continue;
		}

		printf("server:got new conncetion\n");
		
		send(new_fd,"123",3,0);
		printf("send normal data to client:123\n");
		sleep(1);

		send(new_fd,"456",3,MSG_OOB);
		printf("send OOB data to client:456\n");
		sleep(1);

		send(new_fd,"78",2,0);
		printf("send normal data to client:78\n");
		sleep(1);
		
		send(new_fd,"90",2,MSG_OOB);
		printf("send OOB data to client:90\n");
		sleep(1);

		close(new_fd);
	}

	close(sockfd);
	return 0;
}

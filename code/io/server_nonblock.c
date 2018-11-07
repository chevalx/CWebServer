/*server_nonblock.c*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <strings.h>
#include <string.h>

#define PORT 3000

int main(int argc,char **argv)
{
	int sockfd,new_fd;
	struct sockaddr_in srvaddr;

	//create TCP socket
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)
	{	
		printf("socket error\n");
		exit(1);
	}
	
	//construct server socket address
	bzero(&srvaddr,sizeof(srvaddr));
	srvaddr.sin_family=AF_INET;
	srvaddr.sin_port=htons(PORT);
	srvaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	
	//bind socket
	if(bind(sockfd,(struct sockaddr *)&srvaddr,sizeof(struct sockaddr))==-1)
	{
		printf("bind error\n");
		close(sockfd);
		exit(1);
	}
	
	//listen on socket
	if(listen(sockfd,5)==-1)
	{
		printf("listen error\n");
		close(sockfd);
		exit(1);
	}

	//set socket to non block mode
	int flag=fcntl(sockfd,F_GETFL,0);
	fcntl(sockfd,F_SETFL,flag|O_NONBLOCK);
	
	int loopcnt=1;	
	//main loop
	for(;;)
	{
		printf("before accept %d\n", loopcnt++);
		new_fd=accept(sockfd,NULL,NULL);
		if(new_fd==-1)
		{
			printf("accept error\n");
			sleep(1);
			continue;
		}
		
		//got a connection
		printf("server:got a conncetion\n");
	
		//send string to client
		if(write(new_fd,"helloworld",strlen("helloworld"))==-1)
			printf("write eorror\n");
		
		close(new_fd);
	}
	close(sockfd);
	return 0;
}

/*client_block.c */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <errno.h>

#define PORT 3000

int main(int argc,char **argv)
{
	int 	sockfd,nbytes;
	char 	recv_buf[1024];
	struct 	sockaddr_in srvaddr;

	//create TCP socket		
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)
	{
		printf("socket error\n");
		exit(1);
	}

	//compose remote socket address	
	bzero(&srvaddr,sizeof(srvaddr));
	srvaddr.sin_family=AF_INET;
	srvaddr.sin_port=htons(PORT);	
	inet_aton("127.0.0.1",&srvaddr.sin_addr);

	printf("before connect\n");	
	//connect remote server
	if(connect(sockfd,(struct sockaddr *)&srvaddr,sizeof(srvaddr))==-1)
	{
		printf("connect error: %s\n", strerror(errno));
		close(sockfd);
		exit(1);
	}
	printf("after connect\n");
	
	bzero(recv_buf,sizeof(recv_buf));
	//receive data from remote server
	nbytes=read(sockfd,recv_buf,sizeof(recv_buf)-1);
	if(nbytes>0)
		printf("read successfully: %s\n",recv_buf);
	else
		printf("read error\n");

	close(sockfd);
	return 0;
}

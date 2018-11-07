/*oobmark_client.c
 *connect remote server,dump normal or OOB data from server*/
 
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <strings.h>
#include <fcntl.h>

#define PORT 3000

int sock_at_mark(int fd);

int main(int argc,char **argv)
{
	int sockfd,nbytes;
	char buf[1024];
	char oob_buf[1024];
	struct sockaddr_in srvaddr;
		
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)
	{
		printf("socket error\n");
		exit(1);
	}
	
	int	oob_inline=1;
	setsockopt(sockfd,SOL_SOCKET,SO_OOBINLINE,&oob_inline,sizeof(oob_inline));

	bzero(&srvaddr,sizeof(srvaddr));
	srvaddr.sin_family=AF_INET;
	srvaddr.sin_port=htons(PORT);	
	inet_aton("127.0.0.1",&srvaddr.sin_addr);
	
	if(connect(sockfd,(struct sockaddr *)&srvaddr,sizeof(srvaddr))==-1)
	{
		printf("connect error\n");
		close(sockfd);
		exit(1);
	}

	do
	{
		int oob_flag=sock_at_mark(sockfd);
		if(oob_flag==1)
		{
			bzero(oob_buf,sizeof(oob_buf));
			if(oob_inline)
			{
				nbytes=read(sockfd,oob_buf,1);
				printf("received %d OOB bytes with read:%s\n",nbytes,oob_buf);
			}
			else
			{
				nbytes=recv(sockfd,oob_buf,1,MSG_OOB);
				printf("received %d OOB bytes with recv:%s\n",nbytes,oob_buf);
			}

			if(!oob_inline)
			{
				bzero(buf,sizeof(buf));
				nbytes=recv(sockfd,buf,sizeof(buf)-1,0);
				if(nbytes>0)
					printf("received %d normal data with recv:%s\n",nbytes,buf);
			}
		}
		else 
		{
			bzero(buf,sizeof(buf));
			nbytes=read(sockfd,buf,sizeof(buf)-1);
			if(nbytes>0)
				printf("received %d normal data with read from server:%s\n",nbytes,buf); 
		}
	}  while(nbytes>0);

	close(sockfd);
	return 0;
}

int sock_at_mark(int fd)
{
	int flag;
	if(ioctl(fd,SIOCATMARK,&flag)<0)
		return -1;
	return (flag!=0?1:0);
}

/*alarmio.c */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>

#define PORT 4000

int timeout_flag=0;
void sigalrm_handler(int signo);

int main(int argc,char **argv)
{
	int sockfd,nbytes;
	char recv_buf[1024];
	struct sockaddr_in srvaddr;
	struct sigaction sa;

	//create UDP socket
	if((sockfd=socket(AF_INET,SOCK_DGRAM,0))==-1)
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

	sa.sa_handler=sigalrm_handler;   
	sa.sa_flags=SA_INTERRUPT;      
	sigemptyset(&sa.sa_mask);            
	sigaction(SIGALRM,&sa,NULL); 
	
	for(;;)
	{
		bzero(recv_buf,sizeof(recv_buf));
		timeout_flag=0;
		alarm(5);//set a clock
		printf("before recvfrom\n");
		nbytes=recvfrom(sockfd,recv_buf,1024,0,NULL,NULL);
		printf("after recvfrom\n");
	
		if(nbytes<0&&errno==EINTR)
		{
			if(timeout_flag==1)
				printf("recvfrom timeout\n");
			else
				continue;//interrupted by other signal
		}
		else if (nbytes>0)
		{
			alarm(0);//cancle a clock
			recv_buf[nbytes]=0;
			printf("recvd:%s\n",recv_buf);
			break;
		}
	}
	close(sockfd);
	return 0;
}

void sigalrm_handler(int signo)
{
	timeout_flag=1;
}

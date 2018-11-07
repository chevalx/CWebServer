/*sigio_server.c */

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>

#define PORT 5001

int	sockfd;
void sigio_handler(int signo);

int main(int argc,char **argv)
{
	int nbytes;
	char recv_buf[1024];
	struct sockaddr_in srvaddr;
	
	//create udp socket
	if((sockfd=socket(AF_INET,SOCK_DGRAM,0))==-1)
	{
		printf("socket error\n");
		exit(1);
	}
	
	bzero(&srvaddr,sizeof(srvaddr));
	srvaddr.sin_family=AF_INET;
	srvaddr.sin_port=htons(PORT);	
	inet_aton("127.0.0.1",&srvaddr.sin_addr);
	
	//bind socket address
	if(bind(sockfd,(struct sockaddr *)&srvaddr,sizeof(srvaddr))==-1)
	{
		printf("bind error\n");
		close(sockfd);
		exit(1);
	}
	
	//setup SIGIO handler
	struct sigaction act;
	act.sa_handler=sigio_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags=0;
	sigaction(SIGIO,&act,NULL);
	
	//set socket's owner
 	fcntl(sockfd,F_SETOWN,getpid());
 	//startup signal drive mode
	int on=1;
 	ioctl(sockfd,FIOASYNC,&on);

	for(;;){
		sleep(1);
	}
	close(sockfd);
	return 0;
}

void sigio_handler(int signo)
{
	struct sockaddr_in client_addr;
	socklen_t	addr_len=sizeof(client_addr);
	char	buf[1024];
	int n;
	
	printf("signal SIGIO received\n");
	bzero(&client_addr,addr_len);
	bzero(buf,sizeof(buf));
	n=recvfrom(sockfd,buf,sizeof(buf),0,(struct sockaddr *)&client_addr,&addr_len);
	if(n>=0)
	{
		printf("recvd:%s\n",buf);
		bzero(buf,sizeof(buf));
		sprintf(buf,"welcome");
		if(sendto(sockfd,buf,strlen(buf),0,(struct sockaddr *)&client_addr,sizeof(client_addr))==-1)
			printf("send error:%s\n",strerror(errno));
		else
			printf("sendout:%s\n",buf);
	}
	else
		printf("recv error:%s",strerror(errno));
}

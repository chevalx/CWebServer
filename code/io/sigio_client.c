/*sigio_client.c */

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
#include <string.h>
#include <strings.h>

#define PORT 5001

int main(int argc,char **argv)
{
	int sockfd,nbytes;
	char buf[1024];
	struct sockaddr_in addr;
	
	//create udp socket
	sockfd=socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd==-1)
	{
		printf("create socket error\n");
		return 1;
	}
	
	bzero(&addr,sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_port=htons(PORT);
	inet_aton("127.0.0.1",&addr.sin_addr);
	
	bzero(buf,sizeof(buf));
	sprintf(buf,"helloworld");
	sendto(sockfd,buf,strlen(buf),0,(struct sockaddr *)&addr,sizeof(addr));
	printf("sendout:%s\n",buf);
	bzero(buf,sizeof(buf));
	nbytes=recvfrom(sockfd,buf,sizeof(buf),0,NULL,NULL);
	if(nbytes>=0)
		printf("recvd:%s\n",buf);

	close(sockfd);
	return 0;
}

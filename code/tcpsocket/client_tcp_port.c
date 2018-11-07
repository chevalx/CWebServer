/* client_tcp_port.c: send "hello" message to server, 
		and get "welcome" response from server 
   parameter: port_number
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXDATASIZE 128

int main(int argc,char **argv)
{
	int sockfd,nbytes;
	char buf[MAXDATASIZE];
	struct sockaddr_in srvaddr;
	int portnumber;

	//check whether port_number parameter exists?
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s portnumber\n", argv[0]);
		exit(1);
	}
	
	if ((portnumber=atoi(argv[1])) < 0)
	{
		fprintf(stderr, "Usage: %s portnumber\n", argv[0]);
		exit(1);
	}

	//create socket
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd==-1){
		printf("can't create socket\n");
		exit(1);
	}
	
	//assign server's sock address
	bzero(&srvaddr,sizeof(srvaddr));
	srvaddr.sin_family=AF_INET;
	srvaddr.sin_port=htons(portnumber);
	if(inet_aton("127.0.0.1",&srvaddr.sin_addr)==0){
		printf("addr convert error\n");
		close(sockfd);
		exit(1);
	}

	//connect remote server
	if(connect(sockfd,(struct sockaddr *)&srvaddr,sizeof(struct sockaddr))==-1){
		printf("connect error\n");
		close(sockfd);
		exit(1);
	}
	
	//send "hello" message to server
	bzero(buf,sizeof(buf));
	sprintf(buf,"hello");
	write(sockfd,buf,strlen(buf));
	printf("send message to server:%s\n", buf);
	
	//receive response from server
	if((nbytes=read(sockfd,buf,MAXDATASIZE))==-1){
		printf("read error\n");
		close(sockfd);
		exit(1);
	}
	buf[nbytes]='\0';
	printf("srv response:%s\n",buf);

	//close socket
	close(sockfd);
	return 0;
}


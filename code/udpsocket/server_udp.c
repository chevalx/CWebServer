/* server_udp.c: 
	(1) receive message from the client and print it 
	(2) get string from standard input and send it to the client */

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_MSG_SIZE 1024
#define SERVER_PORT 3000

int main()
{
	int sockfd,addrlen;
	char msg[MAX_MSG_SIZE];
	struct sockaddr_in srvaddr,clientaddr;

	//create server udp socket
	sockfd=socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd==-1){
		printf("Socket create error\n");
		return -1;
	}
	
	//assign server's socket address
	bzero(&srvaddr,sizeof(srvaddr));
	addrlen=sizeof(srvaddr);
	srvaddr.sin_family=AF_INET;
	srvaddr.sin_port=htons(SERVER_PORT);
	if(inet_aton("127.0.0.1",&srvaddr.sin_addr)==-1){
		printf("addr convert error\n");
		close(sockfd);
		return -1;
	}
	//srvaddr.sin_addr.s_addr=htonl(INADDR_ANY);

	//bind IP address and port
	if(bind(sockfd,(struct sockaddr *)&srvaddr,addrlen)==-1){
		printf("bind error\n");
		close(sockfd);
		return -1;
	}
		
	while(1) {
		bzero(msg, MAX_MSG_SIZE);
	
		//receive message from client and print it
		recvfrom(sockfd,msg,MAX_MSG_SIZE,0,(struct sockaddr *)(&clientaddr),&addrlen);
		fprintf(stdout, "Receive message from client: %s", msg);
		
		bzero(msg, MAX_MSG_SIZE);
		//get message from standard input
		fgets(msg, MAX_MSG_SIZE, stdin);

		//send message to client
		sendto(sockfd,msg,strlen(msg),0,(struct sockaddr *)(&clientaddr),addrlen);
		printf("Server input message: %s", msg);
	}
	
	//close socket
	close(sockfd);
	return 0;
}

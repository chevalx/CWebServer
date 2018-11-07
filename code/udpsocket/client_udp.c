/* client_udp.c: 
	(1) get string from standard input and send it to the server 
	(2) receive message from the server and print it */

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define MAX_BUF_SIZE 1024

int main(int argc,char **argv){
	int sockfd,port,addrlen;
	char buf[MAX_BUF_SIZE];
	struct sockaddr_in srvaddr,clientaddr;
	
	if (argc!=3) {
		printf("Usage:%s server_ip server_port\n", argv[0]);
		return -1;
	}
	
	//convert port number
	if ((port=atoi(argv[2]))<0) {
		printf("port error\n");
		return -1;
	}

	//create client udp socket
	sockfd=socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd==-1){
		printf("Socket create error\n");
		return -1;
	}
	
	//assign server's sock address
	addrlen=sizeof(struct sockaddr_in);
	bzero(&srvaddr,sizeof(srvaddr));
	srvaddr.sin_family=AF_INET;
	srvaddr.sin_port=htons(port);
	//convert IP address
	if(inet_aton(argv[1],&srvaddr.sin_addr)<0){
		printf("IP error\n");
		close(sockfd);
		return -1;
	}

	while(1) {
		bzero(buf, MAX_BUF_SIZE);

		//get message from standard input
		fgets(buf, MAX_BUF_SIZE, stdin);

		//send message to remote server and print it
		sendto(sockfd,buf,strlen(buf),0,(struct sockaddr *)(&srvaddr),addrlen);
		printf("Client input message: %s", buf);
	
		bzero(buf, MAX_BUF_SIZE);	
		//receive message from remote server and print it
		recvfrom(sockfd,buf,MAX_BUF_SIZE,0,(struct sockaddr *)(&clientaddr),&addrlen);
		fprintf(stdout, "Receive message from server: %s", buf);
	}
	
	//close socket
	close(sockfd);
	return 0;
}

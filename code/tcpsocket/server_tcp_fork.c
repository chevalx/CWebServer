/* server_tcp_fork.c: receive hello message from client, 
	and fork a new process to send back "welcome" message to client*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXDATASIZE 128
#define PORT 5000
#define BACKLOG 5

int main(int argc,char **argv)
{
	int sockfd,new_fd,nbytes,sin_size;
	char buf[MAXDATASIZE];
	struct sockaddr_in srvaddr,clientaddr;

	//create socket
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd==-1){
		printf("can't create socket\n");
		exit(1);
	}
	
	//assign socket address
	bzero(&srvaddr,sizeof(srvaddr));
	srvaddr.sin_family=AF_INET;
	srvaddr.sin_port=htons(PORT);
	if(inet_aton("127.0.0.1",&srvaddr.sin_addr)==0){
		printf("addr convert error\n");
		close(sockfd);
		exit(1);
	}
	//srvaddr.sin_addr.s_addr=htonl(INADDR_ANY);

	//bind IP address and port
	if(bind(sockfd,(struct sockaddr *)&srvaddr,sizeof(struct sockaddr))==-1){
		printf("bind error\n");
		close(sockfd);
		exit(1);
	}
	
	//listen on socket
	if(listen(sockfd,BACKLOG)==-1){
		printf("listen error\n");
		close(sockfd);
		exit(1);
	}
	
	for(;;){
		//accept client's connection
		sin_size=sizeof(struct sockaddr_in);
		if((new_fd=accept(sockfd,(struct sockaddr *)&clientaddr,&sin_size))==-1){
			printf("accept error\n");
			continue;
		}

		//receive message
		nbytes=read(new_fd,buf,MAXDATASIZE);
		buf[nbytes]='\0';
		printf("recv message from client:%s\n",buf);

		if (!fork()) { //child process
			close(sockfd);
			//send message
			bzero(buf,sizeof(buf));
			sprintf(buf,"welcome!");
			write(new_fd,buf,strlen(buf));
			printf("send response to client:%s\n", buf);
			close(new_fd);
			exit(0);
		}

		//close socket
		close(new_fd);
	}
	
	//close socket
	close(sockfd);
	return 0;
}


/* client_oob.c
 * send normal or out-of-band tcp packet to server */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

/*server port number*/
#define PORT 4000

int main(int argc, char *argv[])
{
    int sock_fd;
    struct hostent *server;
    struct sockaddr_in server_addr;

    if(argc < 2)
    {
        fprintf(stderr, "Usage: %s <hostname>\n", argv[0]);
        return 1;
    }

    //get host's IP by its name
    if((server = gethostbyname(argv[1])) == NULL)
    {
        herror("gethostbyname");
        return 1;
    }

    //create socket
    if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        return 1;
    }

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr = *((struct in_addr*)server->h_addr);

    //connect remote server
    if(connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("connect");
        close(sock_fd);
        return 1;
    }

    //send "123" as normal data to server
    if(send(sock_fd, "123", 3, 0) == -1)
    {
        perror("send");
        close(sock_fd);
        return 1;
    }
    printf("Send 3 bytes of normal data: 123\n");
    sleep(1);

    //send "456" as OOB data to server
    if(send(sock_fd, "456", 3, MSG_OOB) == -1)
    {
        perror("send");
        close(sock_fd);
        return 1;
    }
    printf("Send 3 byte of OOB data: 456\n");
    sleep(1);

    //send "78" as normal data to server
    if(send(sock_fd, "78", 2, 0) == -1)
    {
        perror("send");
        close(sock_fd);
        return 1;
    }
    printf("Send 2 bytes of normal data: 78\n");
    sleep(1);

    //send "90" as OOB data to server
    if(send(sock_fd, "90", 2, MSG_OOB) == -1)
    {
        perror("send");
        close(sock_fd);
        return 1;
    }
    printf("Send 2 byte of OOB data: 90\n");
    sleep(1);
  
    //send "a" as normal data to server
    if(send(sock_fd, "a", 1, 0) == -1)
    {
        perror("send");
        close(sock_fd);
        return 1;
    }
    printf("Send 1 bytes of normal data: a\n");
    sleep(1);

    //send "b" as OOB data to server
    if(send(sock_fd, "b", 1, MSG_OOB) == -1)
    {
        perror("send");
        close(sock_fd);
        return 1;
    }
    printf("Send 1 bytes of OOB data: b\n");
    sleep(1);

    //send "c" as OOB data to server
    if(send(sock_fd, "c", 1, MSG_OOB) == -1)
    {
        perror("send");
        close(sock_fd);
        return 1;
    }
    printf("Send 1 bytes of OOB data: c\n");
    sleep(1);

    close(sock_fd);
    return 0;
}

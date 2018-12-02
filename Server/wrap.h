#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>
#include <signal.h>
#include <dirent.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>


typedef struct sockaddr SA;

/* Persistent state for the robust I/O (Rio) package */
#define RIO_BUFSIZE 8192
typedef struct {
    int rio_fd;                // 内部缓冲区的描述符
    int rio_cnt;               // 内部缓冲区的未读字节数
    char *rio_bufptr;          // 内部缓冲区的下n个字节
    char rio_buf[RIO_BUFSIZE]; // 内部缓冲区
} rio_t;

// 封装进程管理函数
pid_t Fork(void);
void Execve(const char *filename, char *const argv[], char *const envp[]);
pid_t Wait(int *status);
pid_t Waitpid(pid_t pid, int *iptr, int options);
void Kill(pid_t pid, int signum);
unsigned int Sleep(unsigned int seconds);
void Pause(void);
unsigned int Alarm(unsigned int seconds);
void Setpgid(pid_t pid, pid_t pgid);
pid_t Getpgrp();

// 封装套接字函数
int Socket(int domain, int type, int protocol);
void Setsockopt(int s, int level, int optname, const void *optval, int optlen);
void Bind(int sockfd, SA *my_addr, int addrlen);
void Listen(int s, int backlog);
int Accept(int s, SA *addr, socklen_t *addrlen);
void Connect(int sockfd, SA *serv_addr, int addrlen);

// DNS 相关函数封装
struct hostent *Gethosynamebyname(const char *name);
struct hostent *Gethosynamebyaddr(const char *addr, int len, int type);

// reentrant protocol-independent client/server helpers
int open_clientfd(char *hostname, char *port);
int open_listenfd(char *port);

// Wrappers for reentrant protocol-independent client/server helpers
int Open_clientfd(char *hostname, char *port);
int Open_listenfd(char *port);

// 对错误处理函数的封装
void unix_error(char *msg);
void posix_error(int code, char *msg);
void dns_error(char *msg);
void gai_error(int code, char *msg);
void app_error(char *msg);

// 封装与协议不相关的函数
void Getaddrinfo(const char *node, const char *service,
                 const struct addrinfo *hints, struct addrinfo **res);
void Getnameinfo(const struct sockaddr *sa, socklen_t salen, char *host,
                 size_t hostlen, char *serv, size_t servlen, int flags);
void Freeaddrinfo(struct addrinfo *res);
void Inet_ntop(int af, const void *src, char *dst, socklen_t size);
void Inet_pton(int af, const char *src, void *dst);

// Robust I/O 函数，对原始的I/O函数进行了封装
ssize_t rio_readn(int fd, void *usrbuf, size_t n);
ssize_t rio_writen(int fd, void *usrbuf, size_t n);
void rio_readinitb(rio_t *rp, int fd);
ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t n);
ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);

// 对上面的函数进行再一次封装
ssize_t Rio_readn(int fd, void *usrbuf, size_t n);
ssize_t Rio_writen(int fd, void *usrbuf, size_t n);
void Rio_readinitb(rio_t *rp, int fd);
ssize_t Rio_readnb(rio_t *rp, void *usrbuf, size_t n);
ssize_t Rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);

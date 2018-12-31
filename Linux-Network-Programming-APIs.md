---
title: Linux 网络编程 API 记录
tags:
---

# 结构体

## 套接字地址

### 通用

```c
struct sockaddr {
    uint8_t       sa_len;
    sa_family_t   sa_family;
    char          sa_data[14];  // 地址，针对协议而不同
}
```

### IPv4

```c
struct in_addr {
	in_addr_t     s_addr;  // 32位IP地址，网络字节序
};
struct sockaddr_in {
    uint8_t       sin_len; // 结构体长度
    sa_family_t   sin_family;
    in_port_t     sin_port;  // 端口号，网络字节序
    struct in_addr  sin_addr;
    char          sin_zero[8]; // 用于填充，全零
};
```

# 函数

## 字节相关

bzero函数把目标地址空间（由传入指针决定）的字节置为0，用于把一个套接字地址结构体初始化为0

```c
#include <strings.h>
void bzero(void *dest, size_t nbytes);
// e.g.
struct sockaddr_in sa;
bzero(&sa, sizeof(sa))
```

字节排序函数用于将字节在主机字节序和网络字节序之间转换，`sa.sin_addr.s_addr`里存储的就是网络字节序，在赋值时需要转换，见下面例子。

```c
#include <netinet/in.h>
uint16_t htons(uint16_t host16bitvalue);
uint32_t htonl(uint32_t host32bitvalue);  // 返回网络字节序
uint16_t ntohs(uint16_t net16bitvalue);
uint32_t ntohl(uint32_t net32bitvalue);   // 返回主机字节序

// e.g.
sa.sin_port = htons(server_port);  // 设置套接字的端口
sa.sin_addr.s_addr = htonl(INADDR_ANY);  // 设置地址，INADDR_ANY 为本机的任意网络设备接口
```

我们写的程序还需要进行字符串和网络字节序的二进制之间的转换。`inet_aton`和`inet_ntoa`函数可以完成这项功能。`inet_addr`函数与`inet_aton`相似，但是已被废弃。

```c
#include <arpa/inet.h>
int inet_aton(const char *strptr, struct in_addr *addrptr);  //将字符串strptr转换为网络字节序，并存储在 addrptr 中，字符串有效返回1，否则返回0
char *inet_ntoa(struct in_addr inaddr); //  将套接字地址结构体中的二进制值转换为字符串并返回

// e.g.
if(inet_aton("192.168.0.2", &sa.sin_addr) == 0){
    perror("inet_aton error");
    exit(1);
}
```

## 套接字相关

为了进行网络通信，一个进程要做的第一件事就是调用`socket`函数建立一个套接字。

```c
#include <sys/socket.h>

int socket(int family, int type, int protocol);  
// 该函数返回一个文件描述符，若成功则文件描述符非负，否则为-1。
// family参数指明了协议域，type指明套接字类型，protocol指明所用协议，一般设置为0
// 对于TCP/IP协议族，family设为AF_INET
// 对于TCP, UDP和原始套接字，type参数的设置是不同的，SOCK_STREAM 表示TCP，SOCK_DGRAM 表示 UDP，SOCK_RAW 表示原始套接字

// e.g.
int fd;  // 文件描述符
fd = socket(AF_INET, SOCK_STREAM, 0);
```

创建套接字得到文件描述符后，其实还没有将其与套接字结构体连接起来。`bind`函数同时接受文件描述符和套接字结构体作为参数，将他们俩关联在一起。

```c
#include <sys/socket.h>

int bind(int sockfd, const struct sockaddr *myaddr, socklen_t addrlen);
```

这里需要补充说明`socklen_t`这个类型。它不是一个结构体，而是POSIX规范要求的一个数据类型，表示套接字地址结构体的长度，一般为`uint32_t`类型。它的头文件为`<sys/socket.h>`。

服务端都需要创建和绑定套接字，TCP客户端则不需要绑定套接字，直接调用`connect`函数建立与服务器的连接即可。

```c
#include <sys/socket.h>
int connect(int sockfd, const struct sockaddr *servaddr, socklen_t addrlen);

// 成功则返回0， 出错则返回-1
// 调用该函数会激发TCP三次握手的过程，出错返回可能有若干种情况
```

服务器在绑定套接字后，需要调用`listen`函数对该套接字进行监听。需要注意的是，套接字在刚被创建时，内核认为它是一个主动套接字，而`listen`函数改变了这个状态，将它转换为被动套接字，指示内核应该接受针对该套接字的连接请求。

```c
#include <sys/socket.h>
int listen(int sockfd, int backlog);

// 成功为0，错误为-1
// backlog 为连接请求队列的长度
```

接下来服务器会调用`accept`函数接收客户端的连接。

```c
#include <sys/socket.h>

int accept(int sockfd, struct sockaddr *cliaddr, socklen_t *addrlen);
// 成功则返回一个新的非负描述符，出错返回-1
// 需要注意，在之后服务器与客户端通信时，使用的是这个新的描述符而不是socket函数返回的那个
```


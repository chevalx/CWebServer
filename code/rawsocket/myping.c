/************************************************/
/*** myping.c                                  ***/
/*** Use the ICMP protocol to request echo reply*/ 
/*                  message from destination. ***/
/************************************************/

#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PACKETSIZE	64
struct packet
{
	struct icmphdr hdr;
	char msg[PACKETSIZE-sizeof(struct icmphdr)];
};

int pid=-1;
struct protoent *proto=NULL;

/*----------------------------------------------*/
/*--checksum - calculate ICMP packet checksum --*/
/*----------------------------------------------*/
unsigned short checksum(void *b, int len)
{	
	unsigned short *buf = b;
	unsigned int sum=0;
	unsigned short result;

	for ( sum = 0; len > 1; len -= 2 )
		sum += *buf++;
	if ( len == 1 )
		sum += *(unsigned char*)buf;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	result = ~sum;
	return result;
}

/*----------------------------------------------*/
/*--- display - print echo reply message     ---*/
/*----------------------------------------------*/
void display(void *buf, int bytes)
{	
	int i;
	struct iphdr *ip = buf;
	struct icmphdr *icmp = buf+ip->ihl*4;
	struct in_addr saddr, daddr;

	printf("\n*************Received Reply Message**************\n");
	saddr.s_addr = ip->saddr;
	daddr.s_addr = ip->daddr;
	printf("IPv%d: hdr-size=%d pkt-size=%d protocol=%d TTL=%d src=%s ",
		ip->version, ip->ihl*4, ntohs(ip->tot_len), ip->protocol,
		ip->ttl, (char *)inet_ntoa(saddr));
	printf("dst=%s\n", (char *)inet_ntoa(daddr));

	if (icmp->un.echo.id == pid)
	{
		printf("ICMP: type[%d/%d] id[%d] seq[%d]\n",
			icmp->type, icmp->code,	icmp->un.echo.id, 
			icmp->un.echo.sequence);
	}
}

/*----------------------------------------------*/
/*-listener-separate process to receive relpy messages-*/
/*----------------------------------------------*/
void listener(void)
{	
	int sd;
	struct sockaddr_in addr;
	unsigned char buf[1024];

	sd = socket(AF_INET, SOCK_RAW, proto->p_proto);
	if (sd <0 )
	{
		perror("socket");
		return;
	}
	for (;;)
	{
		int bytes, len=sizeof(addr);

		bzero(buf, sizeof(buf));
		bytes = recvfrom(sd, buf, sizeof(buf), 0, (struct sockaddr*)&addr, &len);
		if (bytes > 0)
			display(buf, bytes);
		else
			perror("recvfrom");
	}
	close(sd);
	return;
}

/*----------------------------------------------*/
/*--- ping - Create message and send it.     ---*/
/*----------------------------------------------*/
void ping(struct sockaddr_in *addr)
{	
	const int val=255;
	int i, sd, flags, cnt=1;
	struct packet pckt;
	struct sockaddr_in r_addr;

	sd = socket(AF_INET, SOCK_RAW, proto->p_proto);
	if (sd < 0)
	{
		perror("socket");
		return;
	}
	if (setsockopt(sd, SOL_IP, IP_TTL, &val, sizeof(val))!=0)
	{
		perror("Set TTL option");
		close(sd);
		return;
	}
	
	flags=fcntl(sd,F_GETFL,0);
	flags|=O_NONBLOCK;
	if (fcntl(sd, F_SETFL, flags)!=0)
	{
		perror("Request nonblocking I/O");
		close(sd);
		return;
	}
		
	for (;;)
	{	
		int len=sizeof(r_addr);

		bzero(&pckt, sizeof(pckt));
		pckt.hdr.type = ICMP_ECHO;
		pckt.hdr.un.echo.id = pid;
		for (i = 0; i < sizeof(pckt.msg)-1; i++)
			pckt.msg[i] = i+'0';
		pckt.msg[i] = 0;
		pckt.hdr.un.echo.sequence = cnt++;
		pckt.hdr.checksum = checksum(&pckt, sizeof(pckt));
		if (sendto(sd, &pckt, sizeof(pckt), 0, (struct sockaddr*)addr, sizeof(*addr))<=0)
			perror("sendto");
		else
			printf("\nSend ICMP Message: type[%d/%d] id[%d] seq[%d]\n",
			pckt.hdr.type, pckt.hdr.code, pckt.hdr.un.echo.id, 
			pckt.hdr.un.echo.sequence);
			
		sleep(1);
	}
	close(sd);
}

/*----------------------------------------------*/
/*- main-look up host and start ping processes.-*/
/*----------------------------------------------*/
int main(int count, char *strings[])
{	
	struct hostent *hname;
	struct sockaddr_in addr;

	if (count != 2)
	{
		printf("usage: %s <addr>\n", strings[0]);
		return -1;
	}

	pid = getpid();
	proto = getprotobyname("ICMP");
	hname = gethostbyname(strings[1]);
	bzero(&addr, sizeof(addr));
	addr.sin_family = hname->h_addrtype;
	addr.sin_port = 0;
	addr.sin_addr.s_addr = *(long*)hname->h_addr;
	int forkid = fork();
	if (forkid < 0)
	{
		perror("fork");
		return -1;
	}
	else if (forkid==0)
		ping(&addr);
	else
		listener();

	return 0;
}


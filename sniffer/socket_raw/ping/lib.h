#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <netinet/ip_icmp.h>
#include <resolv.h>

#define PACKETSIZE 64
#define ICMP_REPLY 0
#define ICMP_ECHO 8
#define	ICMP_TIME 11

struct packet_struct{
	struct icmphdr icmp;
	char msg[PACKETSIZE - sizeof(struct icmphdr)];	
};

struct my_ip {
    u_int8_t    ip_vhl;
#define IP_V(ip)    (((ip)->ip_vhl & 0xf0) >> 4)
#define IP_HL(ip)   ((ip)->ip_vhl & 0x0f)
    u_int8_t    ip_tos;
    u_int16_t   ip_len;
    u_int16_t   ip_id;
    u_int16_t   ip_off;
    u_int8_t    ip_ttl;
    u_int8_t    ip_p;
    u_int16_t   ip_sum;
    struct  in_addr ip_src, ip_dst;
}__attribute__((packed));



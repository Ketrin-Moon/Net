#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pcap.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/if_ether.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netinet/ether.h>
#include <netinet/tcp.h>

#define MAXSIZE 1024
#define MTU 1500

u_int16_t etype;

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


struct my_tcphdr{
	u_short source;
	u_short dest;
	u_int16_t seq;
	u_int16_t ack;
	u_int off:4;
	u_char len, flags;
	u_short	win;
	u_short	check;
	u_short	urp;
}__attribute__((packed));

struct my_udphdr{
        u_short	source;
        u_short dest;
        u_int16_t seq;
        u_int16_t ack;
        u_int   off:4;
        u_char  len, flags;
        u_short win;
        u_short check;
        u_short urp;
}__attribute__((packed));

struct ps_header{
	struct in_addr source, dest;
	u_int8_t res;
	u_int8_t prot;
	u_short len;
}__attribute__((packed));

struct check_struct{
	struct ps_header *ps;
	struct my_tcphdr *tcp;
}__attribute__((packed));


struct sockaddr_in source, dest;

void process(u_char *args, struct pcap_pkthdr* pkthdr, const u_char* packet);
void print_ip_header(u_char *args, struct pcap_pkthdr* pkthdr, const u_char* packet);
void ethernet(u_char *args, struct pcap_pkthdr* pkthdr, const u_char* packet);
void printData(const u_char *data, int size);
void ip_checksum(struct my_ip *ip);
unsigned short checksum(unsigned short *addr, unsigned int size);
void print_udp_header(u_char *args, struct pcap_pkthdr *pkthdr, const u_char *packet, u_int16_t len);
void print_tcp_header(u_char *args, struct pcap_pkthdr *pkthdr, const u_char *packet, u_int16_t len, struct my_ip *ip);


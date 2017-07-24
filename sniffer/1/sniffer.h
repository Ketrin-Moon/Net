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

#define MAXSIZE 1024
#define MTU 1500

u_int16_t etype;

struct my_ip {
    u_int8_t    ip_vhl;
    u_int8_t	ip_ihl;
#define IP_V(ip)    (((ip)->ip_vhl & 0xf0) >> 4)
#define IP_HL(ip)   ((ip)->ip_vhl & 0x0f)
    u_int8_t    ip_tos;
    u_int16_t   ip_len;
    u_int16_t   ip_id;
    u_int16_t   ip_off;
#define IP_DF 0x4000
#define IP_MF 0x2000
#define IP_OFFMASK 0x01fff
    u_int8_t    ip_ttl;
    u_int8_t    ip_p;
    u_int16_t   ip_sum;
    struct  in_addr ip_src, ip_dst;
};

struct sockaddr_in source, dest;

void process(u_char *args, struct pcap_pkthdr* pkthdr, const u_char* packet);
void print_ip_header(u_char *args, struct pcap_pkthdr* pkthdr, const u_char* packet);
void ethernet(u_char *args, struct pcap_pkthdr* pkthdr, const u_char* packet);
void printData(const u_char *data, int size);
void ip_checksum(struct my_ip *ip);
unsigned short checksum(unsigned short *addr, unsigned int size);


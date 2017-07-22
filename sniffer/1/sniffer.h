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

struct sockaddr_in source, dest;

void process_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *buf);
void printData(const u_char *data, int size);

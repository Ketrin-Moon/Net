#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pcap.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/in.h>

#define MAXSIZE 100

struct sockaddr_in source, dest;

void process_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *buf);
void printData(const u_char *data, int size);

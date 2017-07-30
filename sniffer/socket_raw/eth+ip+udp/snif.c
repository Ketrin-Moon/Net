#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pcap/pcap.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/if_packet.h>
#include <netinet/if_ether.h>

static unsigned short checksum(unsigned short *addr, unsigned int size){
        register unsigned long sum = 0;
        while(size > 1){
                sum += *addr++;
                size -= 2;
        }
        if(size > 0){
            sum += ((*addr)&htons(0xFF00));
        }
        while(sum >> 16){
    	    sum = (sum & 0xffff) + (sum >> 16);
        }
        sum = ~sum;
        return ((unsigned short)sum);
}


int main()
{
	struct sockaddr_ll server;
	struct ifreq if_idx;
	struct udphdr *udp;
	struct iphdr *ip;
	struct ether_header *eth;
	int sock; 
	int flag = 1;
	int i;
	char dgram[1500];
	char *payload_r;
	char payload[20] = "Hello|I'm client";
	char *packet;
	char packet_r[1500];
	socklen_t len = sizeof(struct sockaddr_ll);
	u_char mac_shost[6] = {0x08, 0x00, 0x27, 0xa8, 0xc1, 0x46};
	u_char mac_dhost[6] = {0x00,0x00,0x00,0x00,0x00,0x00};
	u_char reverse_mac_shost[6];
	u_char reverse_mac_dhost[6];

	memset(dgram, 0, 1500);
	memset(packet_r, 0, 1500);

	sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if(sock < 0){
		perror("socket");
		exit(1);
	}
	if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) == -1){
		perror("setsockopt");
		exit(1);
	}

	strncpy(if_idx.ifr_name, "lo", IFNAMSIZ-1);
        if(ioctl(sock, SIOCGIFINDEX, &if_idx) < 0){
	    perror("ioctl");
	    exit(1);
	}

        server.sll_family = AF_PACKET;
        server.sll_protocol = htons(ETH_P_IP);
        server.sll_ifindex = if_idx.ifr_ifindex;
        server.sll_hatype = ARPHRD_ETHER;
//        server.sll_pkttype = PACKET_MULTICAST;
	server.sll_pkttype = PACKET_OTHERHOST;
        server.sll_halen = ETH_ALEN;


	/*ETHERNET HEADER*/
    
	eth = (struct ether_header*)dgram;
	for(i = 0; i < 5; i++){
		reverse_mac_shost[5-i] = mac_shost[i];
		reverse_mac_dhost[5-i] = mac_dhost[i];
	}
	strcpy(eth->ether_shost, &reverse_mac_shost);
	strcpy(eth->ether_dhost, &reverse_mac_dhost);
	eth->ether_type = htons(ETH_P_IP);
	
	/*IP HEADER*/

	ip = (struct iphdr*)(dgram + sizeof(struct ether_header*));

	ip->version = 4;
	ip->ihl = 5;
	ip->tos = 0;
	ip->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + strlen(payload));
	ip->id = htons(54321);
	ip->frag_off = 0;
	ip->ttl = 255;
	ip->protocol = IPPROTO_UDP;
	ip->check = checksum((unsigned short*)ip, (unsigned int)(ip->ihl<<2));
	ip->saddr = inet_addr("192.168.1.6");
	ip->daddr = inet_addr("192.168.1.6");

	/*UDP HEADER*/

	udp=(struct udphdr*)(dgram + sizeof(struct iphdr));

	udp->source = htons(2525);
	udp->dest = htons(3456);
	udp->len = htons(sizeof(struct udphdr));
	udp->check = 0;

	/*PACKET*/

	packet = malloc(sizeof(struct ether_header) + sizeof(struct iphdr) + sizeof(struct udphdr) + strlen(payload));

	memcpy(packet, eth, sizeof(struct ether_header));
	memcpy(packet + sizeof(struct ether_header), ip, sizeof(struct iphdr));
	memcpy(packet + sizeof(struct ether_header) + sizeof(struct iphdr), udp, sizeof(struct udphdr));
	memcpy(packet + sizeof(struct ether_header) + sizeof(struct iphdr) + sizeof(struct udphdr), payload, strlen(payload));

	if(sendto(sock, packet, sizeof(struct ether_header) + ntohs(ip->tot_len), 0, (struct sockaddr*)&server, sizeof(server)) == -1){
	    perror("sendto");
	    exit(1);
	}

	while(1){
		if(recvfrom(sock, &packet_r, 1500, 0, (struct sockaddr*)&server, &len) == -1){
		    perror("recvfrom");
		    exit(1);
		}
		payload_r = (char*)(packet_r + sizeof(struct ether_header) + sizeof(struct iphdr) + sizeof(struct udphdr));
		printf("\n------------------------------------IP Header----------------------------------------\n\n");
	        printf("\t|-IP Version : %d\n", ip->version);
	        printf("\t|-Length : %d\n", ip->ihl);
        	printf("\t|-Type of Service : %d\n", ip->tos);
	        printf("\t|-Total Length : %d\n", ntohs(ip->tot_len));
	        printf("\t|-Identifire : %d\n", ip->id);
        	printf("\t|-Protocol : %d\n", ip->protocol);
	        printf("\n-------------------------------------UDP Header--------------------------------------\n\n");
	        printf("\t|-Source Port : %d\n", ntohs(udp->source));
	        printf("\t|-Destination Port : %d\n", ntohs(udp->dest));
        	printf("\t|-UDP Length : %d\n", ntohs(udp->len));
	        printf("\t|-UDP Checksum : %X\n", udp->check);
	        printf("\n-------------------------------------------------------------------------------------\n");
        	printf("\n   Received a package : %s\t(Length : %li)\n", payload_r, strlen(payload_r));
	        printf("\n-------------------------------------------------------------------------------------\n\n");
		memset(payload_r, 0, 1500);
	}
}

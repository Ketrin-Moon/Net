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
#include <netinet/ether.h>
#include <linux/if_packet.h>
#include <sys/ioctl.h>
#include <net/if.h>

int main()
{
	int sock;
	struct sockaddr_ll server_ll;
	char client[100] = "Hello|I'm client.";
	int tot_len;
	char *dgram = NULL;
	char *payload;
	struct ether_header *eth= NULL;
	struct iphdr *ip = NULL;
	struct udphdr *udp = NULL;
	struct ifreq if_idx, if_mac;
	int i;

	memset(&if_idx, '0', sizeof(struct ifreq));

	sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

	if(sock < 0){
		perror("socket");
		exit(1);
	}
	strncpy(&if_idx.ifr_name, "enp0s3", IFNAMSIZ-1);
        if(ioctl(sock, SIOCGIFINDEX, &if_idx) < 0){
                perror("ioctl");
                exit(1);
        }
	strncpy(&if_mac.ifr_name, "enp0s3", IFNAMSIZ-1);
	if(ioctl(sock, SIOCGIFHWADDR, &if_mac) < 0){
		perror("ioctl2");
		exit(1);
	}

	setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, &if_idx, sizeof(if_idx));

	tot_len = sizeof(payload) + sizeof(struct ether_header) + sizeof(struct iphdr) + sizeof(struct udphdr);
	dgram = calloc(tot_len, sizeof(tot_len));
	udp = malloc(sizeof(struct udphdr));
	ip = malloc(sizeof(struct iphdr));
	eth = malloc(sizeof(struct ether_header));

	payload = dgram + sizeof(struct iphdr) + sizeof(struct udphdr) + sizeof(struct ether_header);
	strncpy(payload, client, 100);

	memset(udp, '0', sizeof(struct udphdr));

	udp->source = htons(2525);
	udp->dest = htons(3456);
	udp->len = htons(sizeof(struct udphdr));
	udp->check = 0;

	memset(ip, '0', sizeof(struct iphdr));

	ip->ihl = 5;
	ip->version = 4;
	ip->tos = 0;
	ip->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + strlen(payload));
	ip->id = htons(35121);
	ip->frag_off = 0;
	ip->ttl = 255;
	ip->protocol = IPPROTO_UDP;
	ip->check = 0;
	ip->saddr = inet_addr("10.0.2.15");
	ip->daddr = inet_addr("127.0.0.1");

	unsigned char *mac;
	mac = (unsigned char *)if_mac.ifr_hwaddr.sa_data;
	printf("MAC-address : %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	unsigned char new_mac[6];
	int count = 5;
	for(i = 0; i < 6; i++){
	    new_mac[count] = mac[i];
	    count--;
	}
	printf("MAC-address : %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n", new_mac[0], new_mac[1], new_mac[2], new_mac[3], new_mac[4], new_mac[5]);
	//strcpy(eth->ether_shost, mac);
	//strcpy(eth->ether_dhost, mac);

	for(i = 0; i < 6; i++){
	    eth->ether_dhost[i] = mac[i];
	    eth->ether_shost[i] = mac[i];
	}

	eth->ether_type = htons(ETH_P_IP);

	server_ll.sll_family = AF_PACKET;
	server_ll.sll_protocol = htons(ETH_P_IP);
	server_ll.sll_ifindex = if_idx.ifr_ifindex;
	server_ll.sll_hatype = ARPHRD_ETHER;
	server_ll.sll_pkttype = PACKET_OTHERHOST;
	server_ll.sll_halen = ETH_ALEN;

//	for(i = 0; i < 6; i++){
//		server_ll.sll_addr[i] = mac[i];
//	}
//	server_ll.sll_addr[6] = 0;
//	server_ll.sll_addr[7] = 8;

	memcpy(dgram, eth, sizeof(struct ether_header));
	memcpy(dgram + sizeof(struct ether_header), ip, sizeof(struct iphdr));
	memcpy(dgram + sizeof(struct ether_header) + sizeof(struct iphdr), udp, sizeof(struct udphdr));
	memcpy(dgram + sizeof(struct ether_header) + sizeof(struct iphdr) + sizeof(struct udphdr), payload, strlen(payload));

	char *buffer;
	buffer=(char*)(dgram + sizeof(struct ether_header) + sizeof(struct iphdr) + sizeof(struct udphdr));
	printf("PAYLOAD : %s\n", buffer);

	if(sendto(sock, dgram, sizeof(struct ether_header)+htons(ip->tot_len), 0, (struct sockaddr*)&server_ll, sizeof(struct sockaddr_ll)) < 0){
		perror("send");
		exit(1);
	}
	printf("\n-------------------------------------------------------------------------------------\n");
	printf("\n   Package sent : %s\t(Length : %li)\n", payload, strlen(payload));

	if(recvfrom(sock, dgram, sizeof(struct ether_header)+ip->tot_len, 0, NULL, NULL) < 0){
		perror("recv");
		exit(1);
	}
	while(1){
		memset(client, '0', 100);

		ip = (struct iphdr *)dgram;
		udp = (struct udphdr *)(dgram + ip->ihl*4);
		if(udp->dest == 2525)
			break;
	}

	memcpy(client, dgram + sizeof(struct udphdr) + ip->ihl*4, 100);

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
	printf("\n   Received a package : %s\t(Length : %li)\n", client, strlen(client));
	printf("\n-------------------------------------------------------------------------------------\n\n");
	return 0;
}

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

int main()
{
	int sock;
	struct sockaddr_in server, cl;
	char client[100] = "Hello|I'm client.";
	char *buf;
	char dgram[100];
	char *payload;
	struct iphdr *ip = NULL;
	struct udphdr *udp = NULL;
	int f = 1;
	socklen_t *len = sizeof(struct sockaddr);

	buf = malloc(100);
	memset(dgram, '0', 100);

	sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);

	if(sock < 0){
		perror("socket");
		exit(1);
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(2525);
	server.sin_addr.s_addr = inet_addr("192.168.2.1");

	udp = (struct udphdr *)(dgram + sizeof(struct iphdr));
	ip = (struct iphdr *)dgram;

	payload = dgram + sizeof(struct iphdr) + sizeof(struct udphdr);
	strncpy(payload, client, 20);

	memset(udp, '0', sizeof(struct udphdr));

	udp->source = htons(2525);
	udp->dest = htons(3456);
	udp->len = htons(sizeof(struct udphdr) + strlen(payload));
	udp->check = 0;

	memset(ip, '0', sizeof(struct iphdr));

	ip->ihl = 5;
	ip->version = 4;
	ip->tos = 0;
	ip->tot_len = sizeof(struct iphdr) + sizeof(struct udphdr) + strlen(payload);
	ip->id = htons(35121);
	ip->frag_off = 0;
	ip->ttl = 255;
	ip->protocol = IPPROTO_UDP;
	ip->check = 0;
	ip->saddr = inet_addr("192.168.2.1");
	ip->daddr = inet_addr("192.168.2.1");

	if(setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &f, sizeof(f)) < 0){
                perror("setsockopt");
                exit(1);
        }

	memcpy(dgram, ip, sizeof(struct iphdr));
	memcpy(dgram + sizeof(struct iphdr), udp, sizeof(struct udphdr));
	memcpy(dgram + sizeof(struct iphdr) + sizeof(struct udphdr), payload, strlen(payload));

	if(sendto(sock, dgram, sizeof(dgram), 0, (struct sockaddr*)&server, sizeof(server)) < 0){
		perror("send");
		exit(1);
	}
	else
		printf("\n-------------------------------------------------------------------------------------\n");
		printf("\n   Package sent : %s\t(Length : %d)\n", payload, strlen(payload));

		if(recvfrom(sock, dgram, sizeof(dgram), 0, (struct sockaddr *)&server, &len) < 0){
			perror("recv");
			exit(1);
	}
	memset(client, '0', 100);

	ip = (struct iphdr *)dgram;
	udp = (struct udphdr *)(dgram + ip->ihl*4);

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
	printf("\n   Received a package : %s\t(Length : %d)\n", client, strlen(client));
	printf("\n-------------------------------------------------------------------------------------\n\n");
	return 0;
}

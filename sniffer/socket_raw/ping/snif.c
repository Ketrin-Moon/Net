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

struct icmphdr{
	u_char type;
	u_char code;
	u_short csum;
	u_short ident;
	u_short seq;
};


int main()
{
	int sock;
	struct sockaddr_in server, cl;
	char client[100] = "Hello|I'm client.";
	//char *buf;
	int tot_len;
	char *dgram = NULL;
	char *payload;
	struct iphdr *ip = NULL;
	struct icmphdr *icmp = NULL;
	int f = 1;
	int pid = 0;
	socklen_t *len = sizeof(struct sockaddr);

	sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

	if(sock < 0){
		perror("socket");
		exit(1);
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(6565);
	server.sin_addr.s_addr = inet_addr("8.8.8.8");

	fcntl(sock, F_SETFL, O_NONBLOCK);

	tot_len = sizeof(struct icmphdr) + sizeof(struct iphdr) + sizeof(payload);
	dgram = calloc(tot_len, sizeof(tot_len));
	ip = malloc(sizeof(struct iphdr));
	icmp = malloc(sizeof(struct icmphdr));

	payload = dgram + sizeof(struct iphdr)  + sizeof(struct icmphdr);
	strncpy(payload, client, 20);

	memset(icmp, '0', sizeof(struct icmphdr));

	icmp->type = 8;
	icmp->code = 0;
	icmp->csum = 0;
	icmp->ident = htons(getpid());
	icmp->seq = htons((icmp->seq)++);

	pid = icmp->ident;

	memset(ip, '0', sizeof(struct iphdr));

	ip->ihl = 5;
	ip->version = 4;
	ip->tos = 0;
	ip->tot_len = htons(sizeof(struct iphdr) + sizeof(struct icmphdr));
	ip->id = htons(35121);
	ip->frag_off = 0;
	ip->ttl = 255;
	ip->protocol = IPPROTO_ICMP;
	ip->check = 0;
	ip->saddr = inet_addr("192.168.2.1");
	ip->daddr = inet_addr("8.8.8.8");

	if(setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &f, sizeof(f)) < 0){
                perror("setsockopt");
                exit(1);
        }
	printf("Size IP : %d\n", sizeof(struct iphdr));
	printf("Size ICMP : %d\n", sizeof(struct icmphdr));
	printf("Size PAYLOAD : %d\n", sizeof(payload));
	printf("Size tot_len : %d\n", tot_len);


        memcpy(dgram, ip, sizeof(struct iphdr));
        memcpy(dgram + sizeof(struct iphdr), icmp, sizeof(struct icmphdr));
        memcpy(dgram + sizeof(struct iphdr) + sizeof(struct icmphdr), payload, strlen(payload));

	int bytes;

//	while(1){
	if(sendto(sock, dgram, tot_len, 0, (struct sockaddr*)&server, sizeof(server)) < 0){
		perror("send");
		exit(1);
	}
	free(icmp);
	free(ip);
	icmp = malloc(sizeof(struct icmphdr));
	ip = malloc(sizeof(struct iphdr));
	bzero(icmp, sizeof(struct icmphdr));
//	printf("TYPE : %d\tCODE : %d\tID : %d\tSEQ : %d\n", icmp->type, icmp->code, icmp->ident, icmp->seq);
	bzero(ip, sizeof(struct iphdr));
//	printf("\t|-IP Version : %d\n", ip->version);
//	printf("\t|-Length : %d\n", ip->ihl);
//	printf("\t|-Type of Service : %d\n", ip->tos);

	sleep(1);

//	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &f, sizeof(f));

	while(1){
	if(bytes = recvfrom(sock, dgram, tot_len, 0, (struct sockaddr *)&server, &len) < 0){
		perror("recv");
		exit(1);
	}
	printf("%d bytes from %d : icmp_req = %d  ttl = %d\n", bytes, ip->daddr, icmp->seq, ip->ttl);
	}
	return 0;
}

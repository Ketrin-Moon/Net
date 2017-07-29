#include "lib.h"

unsigned short checksum(void *b, int len)
{
	u_short *buf = b, result;
	u_int sum = 0;

	for(sum = 0; len > 1; len -=2)
	    sum += *buf++;
	if(len == 1)
	    sum += *(unsigned char *)buf;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	result = ~sum;

	return result;
}

int main(int argc, char **argv)
{
	struct sockaddr_in request, reply;
	struct packet_struct pckt;
	struct hostent *hname = NULL;
	struct my_ip *ip = NULL;
        int sock;
	int ttl = 0;
	int i;
	char buf[100];

	if(argc < 2){
	    printf("./traceroute <address>\n");
	    exit(1);
	}
	sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if(sock < 0){
	    perror("sock");
	    exit(1);
	}
	if(fcntl(sock, F_SETFL, O_NONBLOCK) < 0){
	    perror("fcntl");
	    exit(1);
	}
	request.sin_family = AF_INET;
        request.sin_port = 0;
	request.sin_addr.s_addr = inet_addr(argv[1]);

	while(reply.sin_addr.s_addr != request.sin_addr.s_addr){
	    ttl++;
	    if(setsockopt(sock, SOL_IP, IP_TTL, &ttl, sizeof(ttl)) < 0){
		perror("setsockopt");
		exit(1);
    	    }
	
	    bzero(&pckt, sizeof(pckt));    
	    pckt.icmp.type = ICMP_ECHO;
	    pckt.icmp.un.echo.id = getpid();
	    for(i = 0; i < sizeof(pckt.msg); i++)
		pckt.msg[i] = 0;
	    pckt.icmp.un.echo.sequence = ttl;
	    pckt.icmp.checksum = checksum(&pckt, sizeof(pckt));
	    if(sendto(sock, &pckt, sizeof(pckt), 0, (struct sockaddr *)&request, sizeof(request)) < 0){
		perror("sendto");
		exit(1);
	    }
	    sleep(1);
	    socklen_t len = sizeof(struct sockaddr);
	    if(recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr*)&reply, &len) < 0){
		printf("%d\t---.---.---.---\tПревышен интервал ожидания для запроса\n", ttl);
		continue;
	    }
	    ip = (void *)buf;
	    hname = gethostbyaddr(&reply.sin_addr.s_addr, sizeof(reply.sin_addr.s_addr), reply.sin_family);
	    if(hname < 0){
	    	perror("gethostbyaddr");
		exit(1);
	    }
	    if(hname == 0)
		printf("%d\t%s\t*\n", ttl, inet_ntoa(ip->ip_src));
    	    else
		if(reply.sin_addr.s_addr != request.sin_addr.s_addr)
	    		printf("%d\t%s\t%s\n", ttl, inet_ntoa(ip->ip_src), hname->h_name);
	}
	printf("Трассировка завершена\n");
	return 0;
}

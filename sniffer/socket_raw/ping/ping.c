#include "lib.h"

unsigned short checksum(void *b, int len)
{
	u_short *buf = b, result;
	u_int sum = 0;

	for(sum = 0; len > 1; len -= 2)
	    sum += *buf++;
	if(len == 1)
	    sum += *(unsigned char*)buf;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	result = ~sum;

	return result;
}

void request(struct sockaddr_in *req)
{
	struct packet_struct pckt;
	struct sockaddr_in *rep;
        int socket_req;
        int ttl = 255;
	int i;
	int count = 1;

	socket_req = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

	if(socket_req < 0){
    	    perror("socket_req");
	    exit(1);
        }

	if(fcntl(socket_req, F_SETFL, O_NONBLOCK) < 0){
	    perror("fcntl");
	    exit(1);
        }

	if(setsockopt(socket_req, SOL_IP, IP_TTL, &ttl, sizeof(ttl)) < 0){
	    perror("setsockopt");
	    exit(1);
	}

	for(;;){
	    bzero(&pckt, sizeof(pckt));
	    pckt.icmp.type = ICMP_ECHO;
	    pckt.icmp.un.echo.id = getpid();
	    for(i = 0; i < sizeof(pckt.msg)-1; i++)
		pckt.msg[i] = 0;
	    pckt.icmp.un.echo.sequence = count++;
	    pckt.icmp.checksum = checksum(&pckt, sizeof(pckt));

	    if(sendto(socket_req, &pckt, sizeof(pckt), 0, (struct sockaddr *)req, sizeof(*req)) < 0){
		perror("sendto");
		exit(1);
	    }
	    sleep(1);
	}
}

void reply(pid_t pid)
{
	struct sockaddr_in *rep;
	struct my_ip *ip = NULL;
	struct icmphdr *icmp = NULL;
    	int socket_reply;
	int bytes;
	char buf[100];
	socklen_t len = sizeof(struct sockaddr);

	socket_reply = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if(socket_reply < 0){
	    perror("socket_reply");
	    exit(1);
	}
	for(;;){
	    bzero(buf, sizeof(buf));
	    bytes = recvfrom(socket_reply, buf, sizeof(buf), 0, (struct sockaddr *)&rep, &len);
	    if(bytes < 0){
		perror("recvfrom");
		exit(1);
	    }
	    ip = buf;
	    icmp = buf + IP_HL(ip)*4;
	    if(icmp->un.echo.id == pid){
		    printf("%d bytes from %s: icmp_seq=%d ttl=%d\n", bytes, inet_ntoa(ip->ip_src), icmp->un.echo.sequence, ip->ip_ttl);
	    }
	}
}

int main(int argc, char **argv)
{
	struct sockaddr_in client;
	pid_t pid;

	if(argc < 2){
	    printf("./ping <address>\n");
	    exit(1);
	}

	client.sin_family = AF_INET;
	client.sin_port = 0;
	client.sin_addr.s_addr = inet_addr(argv[1]);
	
	pid = getpid();

	if(fork() == 0){
	    reply(pid);
	}
	else
	    request(&client);
	wait(0);
        return 0;
}


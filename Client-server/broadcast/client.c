#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <netdb.h>

int main()
{
	int sock;
	char buf[15];
	struct sockaddr_in client;

	socklen_t len = sizeof(struct sockaddr);
	memset(buf, '0', 15);

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock == -1){
		perror("socket");
		exit(1);
	}
	client.sin_family = AF_INET;
	client.sin_port = htons(7777);
	client.sin_addr.s_addr = inet_addr("192.168.2.255");

	if(bind(sock, (struct sockaddr *)&client, sizeof(client)) < 0){
		perror("bind");
		exit(1);
	}
	while(1){
		if(recvfrom(sock, buf, 15, 0, (struct sockaddr *)&client, &len) < 0){
			perror("sendto");
			exit(1);
		}
		printf("Принял дейтаграмму: %s\n", buf);
	}


	return 0;
}

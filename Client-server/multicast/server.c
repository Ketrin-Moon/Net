#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>

int main()
{
	int sock;
	char str[10] = "Multicast";
	struct sockaddr_in server;

	socklen_t len = sizeof(struct sockaddr);

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock == -1){
		perror("socket");
		exit(1);
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(7777);
	server.sin_addr.s_addr = inet_addr("224.0.0.13");

	while(1){
		if(sendto(sock, str, 10, 0, (struct sockaddr *)&server, len) < 0){
			perror("sendto");
			exit(1);
		}
		printf("Мультикастовая рассылка...\n");
		sleep(1);
	}
	return 0;
}

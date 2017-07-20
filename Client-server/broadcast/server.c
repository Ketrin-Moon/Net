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
	int flag = 1;
	char str[10] = "Broadcast";
	struct sockaddr_in server;

	socklen_t len = sizeof(struct sockaddr);

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock == -1){
		perror("socket");
		exit(1);
	}

	setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &flag, sizeof(flag));

	server.sin_family = AF_INET;
	server.sin_port = htons(7777);
	server.sin_addr.s_addr = inet_addr("192.168.2.255");


/*	if(bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0){
		perror("bind");
		exit(1);
	}
*/
	while(1){
		if(sendto(sock, str, 10, 0, (struct sockaddr *)&server, len) < 0){
			perror("sendto");
			exit(1);
		}
		printf("Отправление дейтаграммы...\n");
		sleep(1);
	}
	return 0;
}

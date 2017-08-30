#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/un.h>

int main()
{
	int sock;
	struct sockaddr_in address;
	char name_client[20] = "Client";
	char *buf;

	buf = malloc(20);

	sock = socket(AF_INET, SOCK_DGRAM, 0);

	if(sock < 0){
		perror("socket");
		exit(1);
	}
	address.sin_family = AF_INET;
	address.sin_port = htons(3456);
	address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	if(connect(sock, (struct sockaddr *)&address, sizeof(address)) < 0){
		perror("connect");
		exit(1);
	}
	printf("Отправка сообщения серверу...\n");
	send(sock, name_client, sizeof(name_client), 0);
	printf("Ожидается ответ...\n");

	recv(sock, buf, 30, 0);
	printf("Ответ сервера: %s\n", buf);

	return 0;
}

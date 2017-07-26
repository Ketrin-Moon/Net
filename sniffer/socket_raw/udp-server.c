#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <sys/un.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

int main()
{
	int sock;
	struct sockaddr_in addr, client;
	char buf[4096];
	char req[20] = "Hello|I'm client.";
	char rep[20] = "Hi, client";
	char name[20];
	socklen_t len = sizeof(struct sockaddr);

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0){
		perror("socket");
		exit(1);
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(3456);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0){
		perror("bind");
		exit(1);
	}
	while(1){
	printf("Подключение клиента...\n");
	if(recvfrom(sock, buf, 4096, 0, (struct sockaddr *)&client, &len) == -1){
		perror("recvfrom");
		exit(1);
	}
	if(strncmp(buf, req, 20) == 0){
		strncpy(name, rep, 20);
	}
	printf("Принял пакет: %s\t(Length : %d)\nОтвет сервера...\n", buf, strlen(buf));
	if(sendto(sock, name, 4096, 0, (struct sockaddr *)&client, len) == -1){
		perror("sendto");
		exit(1);
	}
	printf("Отправил сообщение : %s\n", name);

	}
	printf("Закрытие сокета...\n");

	close(sock);

	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <sys/un.h>

int main()
{
	int sock;
	struct sockaddr_in addr, client;
	char buf[50];
	char name[20] = "Connected";
	socklen_t len = sizeof(struct sockaddr);

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0){
		perror("socket");
		exit(1);
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(3535);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0){
		perror("bind");
		exit(1);
	}
	printf("Подключение клиента...\n");
	if(recvfrom(sock, buf, 30, 0, (struct sockaddr *)&client, &len) == -1){
		perror("recvfrom");
		exit(1);
	}
	printf("Клиент подключен: %s\nОтвет сервера...\n", buf);
	if(sendto(sock, name, 30, 0, (struct sockaddr *)&client, len) == -1){
		perror("sendto");
		exit(1);
	}

//	printf("Закрытие сокета...\n");


//	close(sock);

	return 0;
}

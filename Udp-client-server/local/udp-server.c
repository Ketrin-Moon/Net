#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <sys/un.h>

#define SOCKET_ADDR "/tmp/socket.socket"
#define SOCKET_CL "socket"

int main()
{
	int sock;
	struct sockaddr_un addr, client;
	char buf[50];
	char name[20] = "Connected";
	int len;

	sock = socket(AF_LOCAL, SOCK_DGRAM, 0);
	if(sock < 0){
		perror("socket");
		exit(1);
	}
	addr.sun_family = AF_LOCAL;
	strncpy(addr.sun_path, SOCKET_ADDR, sizeof(addr.sun_path) - 1);
	unlink(SOCKET_ADDR);
	len = sizeof(addr.sun_family) + strlen(addr.sun_path);
	if(bind(sock, (struct sockaddr *)&addr, len) < 0){
		perror("bind");
		exit(1);
	}
	printf("Подключение клиента...\n");
	read(sock, buf, 50);
	printf("Клиент подключен: %s\nОтвет сервера...\n", buf);

//	unlink(SOCKET_CL);

	client.sun_family = AF_LOCAL;
	strncpy(client.sun_path, SOCKET_CL, sizeof(client.sun_path));

	if(connect(sock, (struct sockaddr *)&client, sizeof(client)) < 0){
		perror("bind");
		exit(1);
	}
	write(sock, name, 20);
	printf("Закрытие сокета...\n");


	close(sock);

	return 0;
}

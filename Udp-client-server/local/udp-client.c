#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/un.h>

#define SOCKET_ADDR "/tmp/socket.socket"
#define SOCKET_CL "socket"

int main()
{
	int sock;
	struct sockaddr_un address, server;
	char name_client[20] = "Client";
	char *buf;
	int len;

	buf = malloc(20);
	sock = socket(AF_LOCAL, SOCK_DGRAM, 0);

	if(sock < 0){
		perror("socket");
		exit(1);
	}
	address.sun_family = AF_LOCAL;
	strncpy(address.sun_path, SOCKET_ADDR, sizeof(address.sun_path) - 1);
	len = sizeof(address.sun_family) + strlen(address.sun_path);
	if(connect(sock, (struct sockaddr *)&address, len) < 0){
		perror("connect");
		exit(1);
	}
	printf("Отправка сообщения серверу...\n");
	write(sock, name_client, sizeof(name_client));
	printf("Ожидается ответ...\n");

//	unlink(SOCKET_CL);

	server.sun_family = AF_LOCAL;
	strncpy(server.sun_path, SOCKET_CL, sizeof(server.sun_path));
	if(bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0){
		perror("bind2");
		exit(1);
	}

	read(sock, buf, 30);
	printf("Ответ сервера: %s\n", buf);

	return 0;
}

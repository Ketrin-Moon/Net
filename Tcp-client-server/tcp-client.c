#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <error.h>

#define SOCK_ADDR "socket"

int main()
{
    int sock;
    char name[20] = "Client";
    char buf[20];
    struct sockaddr_un client;

    sock = socket(AF_LOCAL, SOCK_STREAM, 0);
    client.sun_family = AF_LOCAL;
    strncpy(client.sun_path, SOCK_ADDR, sizeof(client.sun_path));
    if(connect(sock, (struct sockaddr *)&client, sizeof(client)) < 0){
	perror("connect");
	exit(1);
    }
    printf("Подключение к серверу...\n");
    if(send(sock, &name, 20, 0) < 0){
	perror("send");
	exit(1);
    }
    printf("Подключено\n");
    printf("Ожидание ответа...\n");
    recv(sock, buf, 20, 0);
    printf("Ответ сервера: %s\n", buf);

    return 0;
}

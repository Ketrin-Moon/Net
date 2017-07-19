#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <stdlib.h>
#include <error.h>

#define SOCK_ADDR "socket"

int main()
{
    int sock, new_sock;
    char str[20] = "Connected";
    char buf[20];
    socklen_t len = sizeof(struct sockaddr);
    struct sockaddr_un client, server;

    sock = socket(AF_LOCAL, SOCK_STREAM, 0);
    if(sock == -1){
	perror("socket");
	exit(1);
    }

    client.sun_family = AF_LOCAL;
    strncpy(client.sun_path, SOCK_ADDR, sizeof(client.sun_path));

    if(bind(sock, (struct sockaddr *)&client, sizeof(client)) < 0){
	perror("bind");
	exit(1);
    }
    listen(sock, 5);
    printf("Подключение клиента...\n");
    new_sock = accept(sock, (struct sockaddr *)&server, &len);

    recv(new_sock, &buf, 20, 0);
    printf("Клиент подключен: %s\nОтвет сервера...\n", buf);
    send(new_sock, str, 20, 0);
    close(sock);
    unlink(SOCK_ADDR);

}

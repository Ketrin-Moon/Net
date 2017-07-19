#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <error.h>

int main()
{
    	int sock;
    	char name[20] = "Client";
    	char buf[20];
    	struct sockaddr_in client;

    	sock = socket(AF_INET, SOCK_STREAM, 0);
	client.sin_family = AF_INET;
	client.sin_port = htons(3537);
	client.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
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

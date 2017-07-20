#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/un.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <pthread.h>

struct msgreq{
        long type;
	char time[20];
	struct sockaddr_in client;
};

struct msgident{
	int port;
	char str[20];
};

void request()
{
	int sock;
	struct sockaddr_in client;
	struct msgident ident;
	char name_client[20] = "Client\n";
	char str[20] = "Identification";
	char buf[20];

	memset(buf, '0', 21);

	sock = socket(AF_INET, SOCK_STREAM, 0);

	if(sock < 0){
		perror("socket");
		exit(1);
	}
	client.sin_family = AF_INET;
	client.sin_port = htons(5555);
	client.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	if(connect(sock, (struct sockaddr *)&client, sizeof(client)) < 0){
		perror("Connect");
		exit(1);
	}
	printf("Отправка сообщения серверу...\n");
	send(sock, name_client, strlen(name_client), 0);
	printf("Ожидается ответ...\n");
	recv(sock, &ident, sizeof(ident), 0);
	close(sock);
	sleep(1);
	if(strcmp(ident.str, str) == 0){
		printf("Идентификация...\n");

		sock = socket(AF_INET, SOCK_STREAM, 0);

		if(sock == -1){
			perror("sock2");
			exit(1);
		}

		client.sin_family = AF_INET;
		printf("Port: %d \n", ident.port);
		client.sin_port = htons(ident.port);
		client.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		printf("Подключение к серверу...\n");

		if(connect(sock, (struct sockaddr *)&client, sizeof(client)) < 0){
			perror("Connect2");
			exit(1);
		}
		if(send(sock, name_client, sizeof(name_client), 0) < 0){
			perror("send");
			exit(1);
		}
		if((recv(sock, &buf, 100, 0)) < 0){
			perror("recv");
			exit(1);
		}
	}
	printf("Ответ сервера: %s  %d \n", ident.str, ident.port);
	printf("Ответ сервера: %s\n", buf);
}

int main()
{
	pthread_t tid[5];
	int i;

	for(i = 0; i < 5; i++){
		pthread_create(&tid[i], NULL, (void *)request, NULL);
	}
	for(i = 0; i < 5; i++){
		pthread_join(tid, NULL);
	}

	return 0;
}

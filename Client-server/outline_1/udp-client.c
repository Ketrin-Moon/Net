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

void request(int *sock)
{
	char buf[20];
	char name_client[20] = "Client";
	memset(buf, '0', 20);
	printf("Отправка сообщения серверу...\n");
        send(*sock, name_client, sizeof(name_client), 0);
        printf("Ожидается ответ...\n");
        recv(*sock, buf, 20, 0);
        printf("Ответ сервера: %s\n", buf);

}

int main()
{
	int sock;
	pthread_t p_req[5];
	struct sockaddr_in client;
	char buf[20];
	int i;

	memset(buf, '0', 20);

	sock = socket(AF_INET, SOCK_DGRAM, 0);

	if(sock < 0){
		perror("socket");
		exit(1);
	}
	client.sin_family = AF_INET;
	client.sin_port = htons(3535);
	client.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	if(connect(sock, (struct sockaddr *)&client, sizeof(client)) < 0){
		perror("Connect");
		exit(1);
	}
	for(i = 0; i < 5; i++){
		pthread_create(&p_req[i], NULL, (void *)request, (int*)&sock);
	}
	for(i = 0; i < 5; i++){
		pthread_join(p_req[i], NULL);
	}
	return 0;
}

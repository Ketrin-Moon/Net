#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <sys/un.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>

#define N 30

struct msgreq{
	long type;
	char time[30];
	struct sockaddr_in client;
};
struct msgvar{
	int request;
	int reply;
};

void getTime(struct msgvar *var)
{
	struct msgreq s_req;
	time_t now;
	static char time_buf[30];
	struct tm *time_loc;

	bzero(time_buf, 30);
	while(1){
	    msgrcv(var->request, &s_req, sizeof(s_req), 1L, 0);
	    s_req.type = 2L;
    	    time(&now);
	    time_loc = localtime(&now);
	    strcpy(s_req.time, asctime(time_loc));
	    strftime(time_buf, 30, "%Y-%m-%e %H:%H:%S\n", time_loc);
	    msgsnd(var->reply, &s_req, sizeof(s_req), 0);
	}
}


int main()
{
	int sock;
	int i;
	key_t ipckey;
	pthread_t p_req[5];
	struct msgreq s_req;
	struct msgvar var;
	struct sockaddr_in server;
	char buf[N];

	socklen_t len = sizeof(struct sockaddr);

	memset(buf, '0', N);

	sock = socket(AF_INET, SOCK_DGRAM, 0);

	if(sock < 0){
		perror("socket");
		exit(1);
	}

	ipckey = ftok("udp-server.c", 'A');
	var.request = msgget(ipckey, IPC_CREAT|0666);
	ipckey = ftok("udp-server.c", 'B');
	var.reply = msgget(ipckey, IPC_CREAT|0666);

	server.sin_family = AF_INET;
	server.sin_port = htons(3535);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0){
		perror("bind");
		exit(1);
	}

	printf("Подключение клиента...\n");
	int status;
	for(i = 0; i < 5; i++){
		status = pthread_create(&p_req[i], NULL, (void *)getTime, (void *)&var);
	}

	while(1){
		if(recvfrom(sock, buf, 30, 0, (struct sockaddr *)&s_req.client, &len) == -1){
			perror("recvfrom");
			exit(1);
		}

		s_req.type = 1L;
		msgsnd(var.request, &s_req, sizeof(s_req), 0);
		msgrcv(var.reply, &s_req, sizeof(s_req), 2L, 0);
		printf("Клиент подключен: %s\nОтвет сервера...\n", buf);

		if(sendto(sock, s_req.time, 30, 0, (struct sockaddr *)&s_req.client, len) == -1){
			perror("sendto");
			exit(1);
		}
		printf("Отправлено...\n");
	}
	pthread_join(p_req[i], (void **)&status);
	printf("Закрытие сокета...\n");
	close(sock);

	return 0;
}

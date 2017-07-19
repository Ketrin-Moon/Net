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
	int n_port;
};

struct msgident{
	int port;
	char str[20];
};

void getTime(struct msgvar *var)
{
	struct msgreq s_req;
	time_t now;
	static char time_buf[30];
	struct tm *time_loc;

	bzero(time_buf, 30);
	msgrcv(var->request, &s_req, sizeof(s_req), 1L, 0);
	s_req.type = 2L;
    	time(&now);
	time_loc = localtime(&now);
	strcpy(s_req.time, asctime(time_loc));
	strftime(time_buf, 30, "%Y-%m-%e %H:%H:%S\n", time_loc);
	msgsnd(var->reply, &s_req, sizeof(s_req), 0);
}

void* new_port(void *var1)
{
	int sock, new_sock;
	struct msgreq s_req;
	struct sockaddr_in srv;
	char n_buf[N];
	struct msgvar  *var = (struct msgvar *)var1;
	socklen_t len = sizeof(struct sockaddr);
//	int n_port = 5556;

	memset(n_buf, '0', N);

	sock = socket(AF_INET, SOCK_STREAM, 0);

	if(sock < 0){
		perror("socket2");
		exit(1);
	}
	srv.sin_family = AF_INET;
	srv.sin_port = htons(var->n_port);
	srv.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(sock, (struct sockaddr *)&srv, sizeof(srv)) < 0){
		perror("bind2");
		exit(1);
	}
	while(1){
	listen(sock, 5);
	printf("Переключение на другой порт...\n");
	if((new_sock = accept(sock, (struct sockaddr *)&srv, &len)) < 0){
		perror("accept2");
		exit(1);
	}
	if(recv(new_sock, &n_buf, N, 0) < 0){
		perror("recv2");
		exit(1);
	}
	printf("Клиент подключен: %s\nОтвет сервера...\n", n_buf);
	getTime(var);
	msgrcv(var->reply, &s_req, sizeof(s_req), 2L, 0);
	printf("Time: %s\n", s_req.time);

	if(send(new_sock, &(s_req.time), N, 0) <  0){
		perror("send2");
		exit(1);
	}
	var->n_port++;
	}
	//close(sock);
}


int main()
{
	int sock, sock1;
	key_t ipckey;
	pthread_t p_req;
	struct msgreq s_req;
	struct msgvar var;
	struct msgident ident;
	struct sockaddr_in server;
	char str[20] = "Identification";
	char buf[N];
	int port = 5555;
	int status;

	socklen_t len = sizeof(struct sockaddr);

	memset(buf, '0', N);

	sock = socket(AF_INET, SOCK_STREAM, 0);

	if(sock < 0){
		perror("socket");
		exit(1);
	}

	ipckey = ftok("tcp-server.c", 'A');
	var.request = msgget(ipckey, IPC_CREAT|0666);
	ipckey = ftok("tcp-server.c", 'B');
	var.reply = msgget(ipckey, IPC_CREAT|0666);

	server.sin_family = AF_INET;
	server.sin_port = htons(5555);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0){
		perror("bind");
		exit(1);
	}
	listen(sock, 5);
	printf("Подключение клиента...\n");
	while(1){
		sock1 = accept(sock, (struct sockaddr *)&server, &len);
		if(recv(sock1, &buf, N, 0) < 0){
			perror("recv");
			exit(1);
		}
		printf("Клиент: %s\n", buf);

		s_req.type = 1L;
		msgsnd(var.request, &s_req, sizeof(s_req), 0);
		ident.port = port + 1;
		var.n_port = ident.port;
		strncpy(ident.str, str, 20);
		printf("Port: %d str: %s\n", ident.port, ident.str);
		send(sock1, &ident, sizeof(ident), 0);
		if(sock1){
			status = pthread_create(&p_req, NULL, new_port, (void *)&var);
			close(sock1);
			port++;
		}
		sleep(3);
	}
	pthread_join(p_req, (void*)&status);

	return 0;
}

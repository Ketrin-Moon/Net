#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <netinet/sctp.h>
#include <string.h>
#include <time.h>

int main()
{
	int sock, new_sock, ret;
	struct sockaddr_in server;
	time_t currTime;
	char buf_time[30];

	memset(buf_time, '0', 30);

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);

	if(sock == -1){
		perror("socket");
		exit(1);
	}
	server.sin_family = AF_INET;
	server.sin_port = htons(3434);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0){
		perror("bind");
		exit(1);
	}
	listen(sock, 5);
	printf("Подключение...\n");
	while(1){
		new_sock = accept(sock, (struct sockaddr *)NULL, NULL);
		if(new_sock == -1){
			perror("accept");
			exit(1);
		}
		currTime = time(NULL);
		snprintf(buf_time, 30, "%s\n", ctime(&currTime));
		printf("Time: %s\n", buf_time);
		ret = sctp_sendmsg(new_sock, buf_time, sizeof(buf_time), NULL, 0, 0, 0, 1, 0, 0);
		if(ret == -1){
			perror("sctp_sendmsg");
			exit(1);
		}
		snprintf(buf_time, 30, "%s\n", asctime(gmtime(&currTime)));
		ret = sctp_sendmsg(new_sock, buf_time, sizeof(buf_time), NULL, 0, 0, 0, 2, 0, 0);
		if(ret == -1){
			perror("sctp_sendmsg2");
			exit(1);
		}
		sleep(2);
	}

	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <sys/un.h>
#include <sys/time.h>
#include <fcntl.h>

#define TIMEOUT 5

int main()
{
	int sock_udp, sock_tcp, n_sock_tcp;
	struct sockaddr_in addr, client;
	struct timeval tv;
	char buf[50];
	char str[20] = "Connected";
	socklen_t len = sizeof(struct sockaddr);
	fd_set set;

	/*TCP*/

	sock_tcp = socket(AF_INET, SOCK_STREAM, 0);

	if(sock_tcp < 0){
		perror("socket");
		exit(1);
	}
	printf("Ожидается подключение клиента...\n");

	addr.sin_family = AF_INET;
	addr.sin_port = htons(3537);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(sock_tcp, (struct sockaddr *)&addr, sizeof(addr)) < 0){
		perror("bind");
		exit(1);
	}

	listen(sock_tcp, 1);

	/*UDP*/

	sock_udp = socket(AF_INET, SOCK_DGRAM, 0);

        if(sock_udp < 0){
                perror("socket");
                exit(1);
        }

        addr.sin_family = AF_INET;
        addr.sin_port = htons(3538);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if(bind(sock_udp, (struct sockaddr *)&addr, sizeof(addr)) < 0){
                perror("bind");
                exit(1);
        }

	while(1){
		FD_ZERO(&set);
		FD_SET(sock_udp, &set);
		FD_SET(sock_tcp, &set);

		tv.tv_sec = TIMEOUT;
		tv.tv_usec = 0;

		if(select(sock_udp + 1, &set, NULL, NULL, &tv) < 0){
			perror("select");
			exit(1);
		}

		/*UDP*/

		if(FD_ISSET(sock_udp, &set)){
			if(recvfrom(sock_udp, buf, 30, 0, (struct sockaddr *)&client, &len) == -1){
				perror("recvfrom");
				exit(1);
			}
			printf("Клиент подключен: %s\nОтвет сервера...\n", buf);
			if(sendto(sock_udp, str, 30, 0, (struct sockaddr *)&client, len) == -1){
				perror("sendto");
				exit(1);
			}
		}

		/*TCP*/

		if(FD_ISSET(sock_tcp, &set)){
			n_sock_tcp = accept(sock_tcp, (struct sockaddr *)&client, &len);
			if(recv(n_sock_tcp, &buf, 20, 0) < 0){
				perror("recv");
				exit(1);
			}
			printf("Клиент подключен: %s\nОтвет сервера...\n", buf);
			if(send(n_sock_tcp, str, 20, 0) < 0){
				perror("send");
				exit(1);
			}
		}
	}
	printf("Закрытие сокета...\n");

	close(sock_tcp);
	close(sock_udp);

	return 0;
}

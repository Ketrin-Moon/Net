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
#include <sys/epoll.h>

int main()
{
	int sock_udp, sock_tcp, n_sock_tcp;
	struct sockaddr_in addr, client;
	struct epoll_event ev[2];
	struct epoll_event *evlist;
	int epfd, npfd, i;
	char buf[50];
	char str[20] = "Connected";
	socklen_t len = sizeof(struct sockaddr);

	printf("Ожидается подключение клиента...\n");

	evlist = malloc(sizeof(struct epoll_event) * 10);

	/*TCP*/

	sock_tcp = socket(AF_INET, SOCK_STREAM, 0);

	if(sock_tcp < 0){
		perror("socket");
		exit(1);
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(3535);
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
        addr.sin_port = htons(3536);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if(bind(sock_udp, (struct sockaddr *)&addr, sizeof(addr)) < 0){
                perror("bind");
                exit(1);
        }

	epfd = epoll_create(10);
	if(epfd == -1){
	    perror("epoll_create");
	    exit(1);
	}

        ev[0].data.fd = sock_udp;
	    ev[0].events = EPOLLIN;

	    ev[1].data.fd = sock_tcp;
	    ev[1].events = EPOLLIN;

	    if(epoll_ctl(epfd, EPOLL_CTL_ADD, sock_udp, &ev[0]) < 0){
		perror("epoll_ctl");
		exit(1);
	    }
	    if(epoll_ctl(epfd, EPOLL_CTL_ADD, sock_tcp, &ev[1]) < 0){
		perror("epoll_ctl2");
		exit(1);
	    }


	while(1){

	    if((npfd = epoll_wait(epfd, evlist, 10, -1)) < 0){
		perror("epoll_wait");
		exit(1);
	    }

		/*UDP*/
	    for(i = 0; i < npfd; i++){
	    if(evlist[i].data.fd == sock_udp){
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
	    else
		/*TCP*/
	    if(evlist[i].data.fd == sock_tcp){
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
	}
	printf("Закрытие сокета...\n");

	close(sock_tcp);
	close(sock_udp);

	return 0;
}

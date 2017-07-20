#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <netinet/sctp.h>

int main()
{
	int sock;
	int flags, in, i;
	struct sockaddr_in client;
	struct sctp_event_subscribe events;
	struct sctp_sndrcvinfo info;
	char buf_time[30];

	memset(buf_time, '0', 30);

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
	if(sock == -1){
		perror("socket");
		exit(1);
	}

	client.sin_family = AF_INET;
	client.sin_port = htons(3434);
	client.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	if(connect(sock, (struct sockaddr *)&client, sizeof(client)) < 0){
		perror("connect");
		exit(1);
	}
	memset((void *)&events, 0, sizeof(events));
	events.sctp_data_io_event = 1;
	setsockopt(sock, SOL_SCTP, SCTP_EVENTS, (const void *)&events, sizeof(events));

	for(i = 0; i < 2; i++){
		in = sctp_recvmsg(sock, &buf_time, sizeof(buf_time), (struct sockaddr *)NULL, 0, &info, &flags);
		if(in == -1){
			perror("sctp_recvmsg");
			exit(1);
		}
		buf_time[in] = 0;
		if(info.sinfo_stream == 1){
			printf("I stream with number %d. I got a message: (local time)%s\n", info.sinfo_stream, buf_time);
		} else if(info.sinfo_stream == 2){
				printf("I stream with number %d. I got a message: (GMT) %s\n", info.sinfo_stream, buf_time);
			}
	}
	close(sock);

	return 0;
}

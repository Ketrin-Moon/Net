//#include <errno.h>
#include <stdio.h>
#include <memory.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <linux/rtnetlink.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/netlink.h>
#include <sys/types.h>
#include <asm/types.h>
#include <net/route.h>
//#include <linux/cred.h>
//#include <linux/pid.h>
#include <linux/sched.h>


#define BUF_SIZE 1024

char gateway[255];

struct route_info{
	struct in_addr dstAddr;
	struct in_addr srcAddr;
	struct in_addr gateway;
	char ifname[20];
};

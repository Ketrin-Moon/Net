#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include <linux/init.h>
#include <net/sock.h>
#include <linux/skbuff.h>

struct sock *nl_sk = NULL;

static void priority_up(struct sk_buff *skb)
{

	struct task_struct *p;
	struct nlmsghdr *nlh;
	pid_t pid = 0;

//	nlh = kmalloc(sizeof(struct nlmsghdr), GFP_KERNEL);
	nlh = (struct nlmsghdr *)skb->data;
	if(!nlh){
		printk(KERN_INFO "Error in kmalloc(nlh)");
		//return -1;
	}
	p = kmalloc(sizeof(struct task_struct), GFP_KERNEL);
	if(!p){
		printk(KERN_INFO "Error in kmalloc(p)");
		//return -1;
	}
	printk(KERN_INFO "Netlink received msg payload:%d\n", (int *)nlmsg_data(nlh));
	for_each_process(p){
//	pid = nlh->nlmsg_pid;
	//pid = (int*)nlmsg_data(nlh);
//	p = find_task_by_vpid(pid);
//	p = pid_task(find_vpid(pid), PIDTYPE_PID);
//		printk(KERN_INFO "pid_nl: %d", nlh->nlmsg_pid);
		printk(KERN_INFO "Pid: %d\n", p->pid);
	}
}

static int __init task_on(void)
{

	struct netlink_kernel_cfg cfg = {
                .input = priority_up,
        };
        printk("Entering: %s\n", __FUNCTION__);
        nl_sk = netlink_kernel_create(&init_net, 17, &cfg);
        if (!nl_sk) {
                printk(KERN_ALERT "Error creating socket.\n");
                return -1;
        }

	return 0;
}

static void __exit task_off(void)
{
	printk(KERN_INFO "Exit");
     	netlink_kernel_release(nl_sk);
}

module_init(task_on);
module_exit(task_off);

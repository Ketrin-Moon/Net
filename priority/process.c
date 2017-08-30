#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netlink.h>

int init_module(void)
{
	struct task_struct *tsk;
	struct nlmsghdr *nlh = NULL;

	for_each_process(tsk){
		if(tsk->pid == nlh->nlmsg_pid)
			tsk->pid = 0;
		printk(KERN_INFO "%s [%d]\n", tsk->comm, tsk->pid);
	}
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "Exit\n");
}

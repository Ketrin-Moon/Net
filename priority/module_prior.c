#include <linux/module.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <linux/pid.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cred.h>
#include <linux/slab.h>
#include <linux/init_task.h>

#define NETLINK_USER 31

struct sock *nl_sk = NULL;

static void hello_nl_recv_msg(struct sk_buff *skb)
{
	struct task_struct *tsk, *p;
	struct user_namespace *ns = current_user_ns();
	struct cred *cred;
	char name[15] = "prior_user";
	kuid_t keuid, ksuid, kfsuid;

        printk(KERN_INFO "Entering: %s\n", __FUNCTION__);

//	cred = (struct cred *)skb->data;

	keuid = make_kuid(ns, 0);
	ksuid = make_kuid(ns, 0);
	kfsuid = make_kuid(ns, 0);

	for_each_process(tsk){
		if(strcmp(tsk->comm, name) == 0){
			printk(KERN_INFO "name task_struct - %s \t pid - %d\n", tsk->comm, tsk->pid);
			p = tsk;
		}
	}
	cred = prepare_creds();
	if(!p)
		return -1;

	cred->euid = keuid;
	cred->suid = ksuid;
	cred->fsuid = kfsuid;

	commit_creds(cred);

	printk(KERN_INFO "I'm got a cred\n");
}

static int __init hello_init(void)
{
        struct netlink_kernel_cfg cfg = {
                .input = hello_nl_recv_msg,
        };
	printk("Entering: %s\n", __FUNCTION__);
        nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);
        if (!nl_sk) {
                printk(KERN_ALERT "Error creating socket.\n");
                return -1;
        }

        return 0;
}

static void __exit hello_exit(void)
{

        printk(KERN_INFO "exiting hello module\n");
        netlink_kernel_release(nl_sk);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");


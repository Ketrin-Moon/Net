#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/udp.h>



unsigned int hook_func(void *hooknum, struct sk_buff *skb, const struct 
nf_hook_state *state)
{
    struct iphdr *ip;
    struct udphdr *udp;

    if (skb->protocol == htons(ETH_P_IP))
    {
	ip = (struct iphdr *)skb_network_header(skb);
	if (ip->version == 4 && ip->protocol == IPPROTO_UDP)
	{
	    skb_set_transport_header(skb, ip->ihl * 4);
	    udp = (struct udphdr *)skb_transport_header(skb);
	    if (udp->dest == htons(3456))
	    {
		return NF_DROP;
	    }
	}
    }
    return NF_ACCEPT;
}

/* Заполняем структуру для регистрации hook функции */

static struct nf_hook_ops nfho = {		/* Структура для регистрации функции перехватчика входящих ip пакетов */
	.hook = hook_func,	        	/* Указываем имя функции, которая будет обрабатывать пакеты */
	.pf = PF_INET,				/* Указываем семейство протоколов */
	.hooknum = NF_INET_PRE_ROUTING,		/* Указываем, в каком месте будет срабатывать функция */
	.priority = NF_IP_PRI_FIRST,		/* Указываем наивысший приоритет*/
};

int Init(void)
{
	printk(KERN_INFO "Start module Shifter\n");

	nf_register_hook(&nfho);		/* Регистрируем */

    	return 0;
}

void Exit(void)
{
	nf_unregister_hook(&nfho);		/* Удаляем из цепочки hook функцию */
	printk(KERN_INFO "End module Shifter\n");
}

module_init(Init);
module_exit(Exit);

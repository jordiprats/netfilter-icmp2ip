/* 
del 224. cap amunt es multicast o reservat
*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/ip.h>
#include <linux/mm.h>
#include <linux/time.h>
#include <linux/err.h>
#include <linux/crypto.h>
#include <linux/init.h>
#include <linux/crypto.h>
#include <linux/scatterlist.h>
#include <net/ip.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/inet.h>
#include <linux/icmp.h>
#include <linux/tcp.h>
#include <linux/in.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("systemadmin.es");

#define MAXIP 30

#define DEBUG 1

typedef struct s_ipttl
{
	unsigned int ip;
	unsigned char ttl;
	unsigned int pos;
	struct timeval lu;
} ipttl;

static struct nf_hook_ops nfhook[2];

static ipttl ip2ttl[MAXIP];

static unsigned int icmp_check(unsigned int hooknum,
				struct sk_buff *skb,
				const struct net_device *in,
				const struct net_device *out,
				int (*okfn)(struct sk_buff *))
{
	struct iphdr *iph;
	struct icmphdr *icmph;
	int i=0;
	int pos=0;
	int candidat=0;
	unsigned long candidat_ts=0;

	if(skb == NULL) return -1;

	iph = ip_hdr(skb);
	if(iph->protocol == IPPROTO_ICMP)
	{
		icmph = icmp_hdr(skb);
		if(icmph->type == ICMP_ECHOREPLY)
		{
			if(DEBUG) printk(KERN_DEBUG "reply\n");
			for(i=0;i<MAXIP;i++)
				if(ip2ttl[i].ip==iph->daddr)
					break;
			
			if(i==MAXIP) return NF_ACCEPT;		
		
			pos=ip2ttl[i].pos;
			
			iph->ttl=((iph->daddr&((unsigned int)255<<(pos*8)))>>(pos*8))+(char)(64-ip2ttl[i].ttl);
			ip2ttl[i].pos=(pos>=3)?0:++pos;
			
			ip_send_check(iph);
		}
		else if((icmph->type == ICMP_ECHO)||(icmph->type == 69))
		{
			if(DEBUG) printk(KERN_DEBUG "echo\n");
			candidat_ts=ip2ttl[0].lu.tv_sec;
			for(i=0;i<MAXIP;i++)
			{
				if((ip2ttl[i].ip==0)||(ip2ttl[i].ip==iph->saddr)) 
					break;
				if(ip2ttl[i].lu.tv_sec<candidat_ts)
				{
					candidat_ts=ip2ttl[i].lu.tv_sec;
					candidat=i;
				}
			}
			
			if(i==MAXIP) i=candidat;
			
			ip2ttl[i].ip=iph->saddr;
			ip2ttl[i].ttl=(iph->ttl>64)?64:iph->ttl;
			do_gettimeofday(&ip2ttl[i].lu);
		}
		else
			if(DEBUG) printk(KERN_DEBUG "icmp type: %d",icmph->type);
	}
	return NF_ACCEPT;
}

static int __init startup(void)
{
	if(DEBUG) printk(KERN_DEBUG "startup icmp2ip\n");
	memset(ip2ttl, 0, sizeof(ipttl)*MAXIP);
	
	nfhook[0].hook     = icmp_check;
	nfhook[0].hooknum  = NF_INET_PRE_ROUTING;
	nfhook[0].pf       = PF_INET;
	nfhook[0].priority = NF_IP_PRI_FILTER;   
	
	nfhook[1].hook     = icmp_check;
	nfhook[1].hooknum  = NF_INET_POST_ROUTING;
	nfhook[1].pf       = PF_INET;
	nfhook[1].priority = NF_IP_PRI_FILTER;   
	
	nf_register_hook(&nfhook[0]);
	nf_register_hook(&nfhook[1]);

	return 0;
}

static void __exit cleanup(void)
{
	nf_unregister_hook(&nfhook[0]);  
	nf_unregister_hook(&nfhook[1]);  
}

module_init(startup);
module_exit(cleanup);

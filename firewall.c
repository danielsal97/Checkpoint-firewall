#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/inet.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define DEVICE_NAME "firewall_device"
#define IOCTL_ADD_IP_RANGE _IOW('a', 1, char *)
#define IOCTL_REMOVE_IP_RANGE _IOW('a', 2, char *)
#define IOCTL_TOGGLE_BLOCKING _IO('a', 3)
#define IOCTL_GET_BLOCKED_IPS _IOR('a', 4, char *)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Netfilter module to block traffic with API support");
MODULE_VERSION("0.1");

static struct nf_hook_ops netfilter_ops_in;
static int blocking_enabled = 1; // Start with blocking enabled

// Structure to hold IP range in linked list
struct ip_range {
    unsigned int start_ip;
    unsigned int end_ip;
    struct list_head list;
};

static LIST_HEAD(ip_ranges); // Linked list to hold dynamic IP ranges

// Helper function to convert a string IP range (e.g., "5.0.0.0-5.0.0.255") into integers
static int parse_ip_range(const char *ip_range_str, unsigned int *start_ip, unsigned int *end_ip)
{
    char start_ip_str[16], end_ip_str[16];
    
    if (sscanf(ip_range_str, "%15[^-]-%15s", start_ip_str, end_ip_str) != 2)
        return -EINVAL;
    
    *start_ip = in_aton(start_ip_str);
    *end_ip = in_aton(end_ip_str);
    
    return 0;
}

// Check if an IP belongs to any range
static int is_blocked_ip(unsigned int ip)
{
    struct ip_range *range;

    list_for_each_entry(range, &ip_ranges, list) {
        if (ip >= range->start_ip && ip <= range->end_ip)
            return 1; // IP is blocked
    }
    return 0;
}

// Netfilter hook function to block IPs
unsigned int block_ips(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
    struct iphdr *ip_header;
    unsigned int src_ip;

    if (!blocking_enabled)
        return NF_ACCEPT;

    ip_header = (struct iphdr *)skb_network_header(skb);
    if (!ip_header)
        return NF_ACCEPT;

    src_ip = ip_header->saddr;
    if (is_blocked_ip(src_ip)) {
        printk(KERN_INFO "Dropping packet from blocked IP: %pI4\n", &src_ip);
        return NF_DROP;
    }

    return NF_ACCEPT;
}

// Handle ioctl commands from user space
static long device_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    char ip_range_str[32];
    char ip_list[512] = "";
    struct ip_range *range, *tmp;
    unsigned int start_ip, end_ip;
    int ret = 0;

    switch (cmd) {
        case IOCTL_ADD_IP_RANGE:
            if (copy_from_user(ip_range_str, (char *)arg, sizeof(ip_range_str)))
                return -EFAULT;

            if (parse_ip_range(ip_range_str, &start_ip, &end_ip) == 0) {
                range = kmalloc(sizeof(*range), GFP_KERNEL);
                if (!range)
                    return -ENOMEM;
                
                range->start_ip = start_ip;
                range->end_ip = end_ip;
                INIT_LIST_HEAD(&range->list);
                list_add(&range->list, &ip_ranges);
                printk(KERN_INFO "Added IP range: %pI4-%pI4\n", &start_ip, &end_ip);
            } else {
                ret = -EINVAL;
            }
            break;

        case IOCTL_REMOVE_IP_RANGE:
            if (copy_from_user(ip_range_str, (char *)arg, sizeof(ip_range_str)))
                return -EFAULT;

            if (parse_ip_range(ip_range_str, &start_ip, &end_ip) == 0) {
                list_for_each_entry_safe(range, tmp, &ip_ranges, list) {
                    if (range->start_ip == start_ip && range->end_ip == end_ip) {
                        list_del(&range->list);
                        kfree(range);
                        printk(KERN_INFO "Removed IP range: %pI4-%pI4\n", &start_ip, &end_ip);
                        break;
                    }
                }
            } else {
                ret = -EINVAL;
            }
            break;

        case IOCTL_TOGGLE_BLOCKING:
            blocking_enabled = !blocking_enabled;
            printk(KERN_INFO "Blocking %s\n", blocking_enabled ? "enabled" : "disabled");
            break;

        case IOCTL_GET_BLOCKED_IPS:
            list_for_each_entry(range, &ip_ranges, list) {
                char range_str[32];
                snprintf(range_str, sizeof(range_str), "%pI4-%pI4\n", &range->start_ip, &range->end_ip);
                strcat(ip_list, range_str);
            }
            if (copy_to_user((char *)arg, ip_list, sizeof(ip_list))) {
                return -EFAULT;
            }
            break;

        default:
            return -EINVAL;
    }

    return ret;
}

// File operations struct for handling IOCTL
static struct file_operations fops = {
    .unlocked_ioctl = device_ioctl,
};

// Initialize the module
static int __init firewall_init(void)
{
    printk(KERN_INFO "Initializing firewall module...\n");

    // Register the device for communication
    register_chrdev(100, DEVICE_NAME, &fops);

    // Register the Netfilter hook
    netfilter_ops_in.hook = block_ips;
    netfilter_ops_in.hooknum = NF_INET_PRE_ROUTING;
    netfilter_ops_in.pf = PF_INET;
    netfilter_ops_in.priority = NF_IP_PRI_FIRST;
    nf_register_net_hook(&init_net, &netfilter_ops_in);

    return 0;
}

// Cleanup the module
static void __exit firewall_exit(void)
{
    struct ip_range *range, *tmp;

    printk(KERN_INFO "Exiting firewall module...\n");

    // Cleanup IP ranges
    list_for_each_entry_safe(range, tmp, &ip_ranges, list) {
        list_del(&range->list);
        kfree(range);
    }

    nf_unregister_net_hook(&init_net, &netfilter_ops_in);
    unregister_chrdev(100, DEVICE_NAME);
}

module_init(firewall_init);
module_exit(firewall_exit);
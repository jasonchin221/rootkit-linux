#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/netdevice.h>


static void *
rl_sys_call_table_old;

int
rl_get_syscall_table(void)
{
    asm("movl $0x8, rl_sys_call_table_old\n\t");

    printk("Syscall table = %p\n", rl_sys_call_table_old);

    return 0;
}

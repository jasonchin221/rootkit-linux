#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/netdevice.h>

#include "rl_syscall.h"

static void *
rl_sys_call_table_old;

static ulong 
rl_get_idt_base(void)
{
     rl_idt_t   idt_table;

     __asm__ __volatile__("sidt %0":"=m"(idt_table));

     return idt_table.it_base;
}

int
rl_get_syscall_table(void)
{
    ulong   idt_base;

    idt_base = rl_get_idt_base();

    printk("idt_base = %x\n", idt_base);

    return 0;
}

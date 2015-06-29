#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/syscalls.h>
#include <linux/kernel.h>

#include "rl_syscall.h"

#define RL_SYSCALL_MAX_NUM      500

static void **
rl_sys_call_table_curr;
static void *
rl_sys_call_table_old[RL_SYSCALL_MAX_NUM];

asmlinkage long 
rl_my_mkdir(const char *name,int mod)
{
    printk(KERN_ALERT"mkdir call is intercepted, name = %s\n", name);

    return 0;
}

static rl_syscall_hijack_t rl_syscall_hijack[] = {
    {__NR_mkdir, rl_my_mkdir},
};

#define rl_syscall_hijack_num \
    (sizeof(rl_syscall_hijack)/sizeof(rl_syscall_hijack[0]))

static void
rl_hijack_syscall(void **table)
{
    u16     i;
    u16     num;

    for (i = 0; i < rl_syscall_hijack_num; i++) {
        num = rl_syscall_hijack[i].sh_syscall_num;
        table[num] = rl_syscall_hijack[i].sh_syscall_func;
    }
}

static void
rl_restore_syscall(void **table)
{
    u16     i;
    u16     num;

    for (i = 0; i < rl_syscall_hijack_num; i++) {
        num = rl_syscall_hijack[i].sh_syscall_num;
        table[num] = rl_sys_call_table_old[num];
    }
}

ulong 
rl_get_sys_call_table(ulong call_entry, char *exp, u32 cope)
{
    char *begin;
    char *end;

    begin = (char *)call_entry;
    end = (char *)(call_entry + cope);

    for (; begin < end; begin++) {
        if (begin[0] == exp[0] && 
                begin[1] == exp[1] && 
                begin[2] == exp[2]) {
            return *((ulong *)(begin + 3));
        }
    }

    return 0;
}

int
rl_modify_syscall_table(void)
{
    ulong   cr0;

    rl_sys_call_table_curr = rl_get_sys_call_table_addr();

    memcpy(rl_sys_call_table_old, 
            rl_sys_call_table_curr, 
            sizeof(rl_sys_call_table_old));  

    cr0 = rl_clear_cr0_save();
    rl_hijack_syscall(rl_sys_call_table_curr);
    rl_setback_cr0(cr0);

    return 0;
}

void
rl_restore_syscall_table(void)
{
    ulong   cr0;

    cr0 = rl_clear_cr0_save();
    rl_restore_syscall(rl_sys_call_table_curr);
    rl_setback_cr0(cr0);
}

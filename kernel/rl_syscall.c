#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/syscalls.h>
#include <linux/kernel.h>

#include "rl_syscall.h"

#define RL_CR0_FLAG     0x10000

static void **
rl_sys_call_table_curr;
static void *
rl_sys_call_table_old[NR_syscalls];

asmlinkage long 
my_mkdir(const char *name,int mod)
{
    printk(KERN_ALERT"mkdir call is intercepted, name = %s\n", name);

    return 0;
}

static rl_syscall_hijack_t rl_syscall_hijack[] = {
    {__NR_mkdir, my_mkdir},
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

static ulong 
rl_get_idt_base(void)
{
     rl_idt_t   idt_table;

     __asm__ __volatile__("sidt %0":"=m"(idt_table));

     return idt_table.it_base;
}

static ulong 
rl_get_sys_call_entry(ulong idt_base)
{
    rl_idt_gate_t   sys_call;
    ulong           sys_call_entry;

    memcpy(&sys_call, (void *)(idt_base + 8*0x80), sizeof(sys_call));
    sys_call_entry = (sys_call.ig_off2 << 16) | sys_call.ig_off1;

    return sys_call_entry;
}

static ulong 
rl_get_sys_call_table(ulong sys_call_entry, char *exp, u32 cope)
{
    char *begin;
    char *end;

    begin = (char *)sys_call_entry;
    end = (char *)(sys_call_entry + cope);

    for (; begin < end; begin++) {
        if (begin[0] == exp[0] && 
                begin[1] == exp[1] && 
                begin[2] == exp[2]) {
            return *((ulong *)(begin + 3));
        }
    }

    return 0;
}

static void 
rl_setback_cr0(ulong val)
{
    __asm__ __volatile__ ("movl %%eax, %%cr0"
            :
            : "a"(val));
}

static ulong 
rl_clear_cr0_save(void)
{
    ulong   cr0 = 0;
    ulong   ret;

    __asm__ __volatile__ ("movl %%cr0, %%eax":"=a"(cr0));
    ret = cr0;

    cr0 &= ~RL_CR0_FLAG;
    __asm__ __volatile__ ("movl %%eax, %%cr0":: "a"(cr0));

    return ret;
}

int
rl_modify_syscall_table(void)
{
    ulong   idt_base;
    ulong   call_entry;
    ulong   call_table;
    ulong   cr0;

    idt_base = rl_get_idt_base();
    call_entry = rl_get_sys_call_entry(idt_base);
    call_table = rl_get_sys_call_table(call_entry, "\xff\x14\x85", 100);
    rl_sys_call_table_curr = (void **)call_table;

    memcpy(rl_sys_call_table_old, 
            rl_sys_call_table_curr, 
            sizeof(rl_sys_call_table_old));  

    //wp clear
    cr0 = rl_clear_cr0_save();
    rl_hijack_syscall(rl_sys_call_table_curr);
    //set wp bit
    rl_setback_cr0(cr0);

    return 0;
}

void
rl_restore_syscall_table(void)
{
    ulong   cr0;

    cr0 = rl_clear_cr0_save();
    memcpy(rl_sys_call_table_curr, 
            rl_sys_call_table_old, 
            sizeof(rl_sys_call_table_old));  
    rl_setback_cr0(cr0);
}

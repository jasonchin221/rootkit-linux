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


struct idt_gate
{
    unsigned short off1;
    unsigned short sel;
    unsigned char nome,flags;
    unsigned short off2;
}__attribute__((packed));

unsigned int get_sys_call_entry(unsigned int idt_base)
{
    struct idt_gate sys_call;
    memcpy(&sys_call,idt_base+8*0x80,sizeof(struct idt_gate));
    unsigned int sys_call_entry=(sys_call.off2 << 16) | sys_call.off1;
    return sys_call_entry;
}

unsigned int get_sys_call_table_entry(unsigned int sys_call_entry,char * exp,unsigned int cope)
{
    char * begin=sys_call_entry;
    char * end=sys_call_entry+cope;
    for(;begin<end;begin++)
    {
        if(begin[0]==exp[0]&&begin[1]==exp[1]&&begin[2]==exp[2])
            return *((unsigned int *)(begin+3));
    }
    return 0;
}


void setback_cr0(unsigned int val)
{
    asm volatile ("movl %%eax, %%cr0"
            :
            : "a"(val)
            );
}

unsigned int clear_cr0_save()
{
    unsigned int cr0 = 0;
    unsigned int ret;
    __asm__ __volatile__ ("movl %%cr0, %%eax":"=a"(cr0));
    ret = cr0;

    cr0 &= 0xfffeffff;
    asm volatile ("movl %%eax, %%cr0":: "a"(cr0));
    return ret;
}

asmlinkage long my_mkdir(const char *name,int mod)
{
    printk(KERN_ALERT"mkdir call is intercepted\n");
}

int
rl_get_syscall_table(void)
{
    ulong   idt_base;
    ulong   call_entry;
    ulong   call_table;

    idt_base = rl_get_idt_base();
    call_entry = get_sys_call_entry(idt_base);
    call_table = get_sys_call_table_entry(call_entry, "\xff\x14\x85", 100);
    void ** table=(void **)call_table;

    printk("call_table = %x\n", table);

    //wp clear
    unsigned int cr0=clear_cr0_save();
    // intercept mkdir call
    table[__NR_mkdir]=my_mkdir;
    //                    //   table[__NR_open]=my_mkoep;
    //set wp bit
    setback_cr0(cr0);

    return 0;
}

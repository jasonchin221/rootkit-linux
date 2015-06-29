#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/syscalls.h>
#include <linux/kernel.h>

#include "rl_syscall.h"

#define RL_CR0_FLAG                 0x10000
#define RL_IA32_SEARCH_SCOPE        100
#define RL_CALL_OPCODE              "\xff\x14\x85"

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

void 
rl_setback_cr0(ulong val)
{
    __asm__ __volatile__ ("movl %%eax, %%cr0"
            :
            : "a"(val));
}

ulong 
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

void *
rl_get_sys_call_table_addr(void)
{
    ulong   idt_base;
    ulong   call_entry;
    ulong   call_table;

    idt_base = rl_get_idt_base();
    call_entry = rl_get_sys_call_entry(idt_base);
    call_table = rl_get_sys_call_table(call_entry, 
            RL_CALL_OPCODE, RL_IA32_SEARCH_SCOPE);

    return (void **)call_table;
}

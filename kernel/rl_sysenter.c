#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <asm/msr.h>

#include "rl_syscall.h"

#define RL_SYSCALL_MAX_NUM      500

static u32 rl_sysenter_old;

asmlinkage char* my_function(void);
unsigned long handler_code=(unsigned long)&my_function;
extern asmlinkage void my_stub(void);

void stub_trtr(void)
{
    __asm__ (".globl my_stub\n\t"
            ".align 4,0x90\n\t"
            "my_stub:\n\t"
            "       call *%0\n\t"
            "       jmp  *%1\n\t"
            :
            :"m"(handler_code),"m"(rl_sysenter_old));
}

asmlinkage char* my_function(void)
{
    int my_eax;
    __asm__("movl %%eax,%0;":"=r"(my_eax));
    printk("Syscall num is %d\n", my_eax);
    return NULL;
}

int
rl_sysenter_hijack(void)
{
    u32     a;
    u32     low;
    u32     high;
    ulong   func;

    rdmsr(MSR_IA32_SYSENTER_EIP, rl_sysenter_old, a);
    func = (ulong)my_stub;
    low = (u32)func;
    high = (u32)((u64)func >> 32);
    wrmsr(MSR_IA32_SYSENTER_EIP, low, high);

    return 0;
}

void
rl_sysenter_restore(void)
{
    wrmsr(MSR_IA32_SYSENTER_EIP, rl_sysenter_old, 0);
}

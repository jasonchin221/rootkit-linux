#ifndef __RL_SYSCALL_H__
#define __RL_SYSCALL_H__

struct _rl_idt_t {
    u16         it_limit;
    ulong       it_base;
}__attribute__((packed));

typedef struct _rl_idt_t rl_idt_t;

extern int rl_get_syscall_table(void);

#endif

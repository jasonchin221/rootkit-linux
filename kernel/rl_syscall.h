#ifndef __RL_SYSCALL_H__
#define __RL_SYSCALL_H__

struct _rl_idt_t {
    u16         it_limit;
    ulong       it_base;
} __attribute__((packed));

typedef struct _rl_idt_t rl_idt_t;

struct _rl_idt_gate_t {
    u16     ig_off1;
    u16     ig_sel;
    u8      ig_nome;
    u8      ig_flags;
    u16     ig_off2;
} __attribute__((packed));

typedef struct _rl_idt_gate_t rl_idt_gate_t;

extern int rl_get_syscall_table(void);

#endif
